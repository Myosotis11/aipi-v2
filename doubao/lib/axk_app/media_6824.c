#include "board.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "bflb_clock.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "bflb_gpio.h"
#include "bflb_uart.h"
#include "bl616_glb.h"

#include "vb6824.h"
#include "ogg.h"
#include "opus/opus.h"
#include "axk_ringbuff.h"
#include "media.h"
#include "tone.h"

#define TX_GPIO 19
#define RX_GPIO 18

#define AU_TASK_STACK_SIZE  (1024 * 12)
#define OPUS_OUT_BUFFER_SIZE 1276 // 1276 bytes is recommended by opus_encode
#define SAMPLE_RATE 16000
#define BUFFER_SAMPLES 320
#define TX_PLAY_THRESHOLD BUFFER_SAMPLES * 5
#define TX_AU_RINGBUFF_SIZE BUFFER_SAMPLES * 8
#define RX_AU_RINGBUFF_SIZE BUFFER_SAMPLES * 8
#define VB6824_OPUS_FRAME_SIZE  40

struct bflb_device_s *dma0_ch0;
static TimerHandle_t write_6824_timer;
static TaskHandle_t au_thread_handle = NULL;
static uint8_t opus_frame[VB6824_OPUS_FRAME_SIZE];
static opus_int16 *output_buffer = NULL;
static OpusDecoder *opus_decoder = NULL;
static volatile int playing_flag;
static int play_tone_id;
static volatile int play_tone_flag;
static int rec_flag;
static TickType_t last_wakeup_tick;

static struct
{
    uint8_t id;
    uint8_t last_play_size;
} tx_buff_mng;

static ring_buff_t au_out_ring_buff;
static char au_out_buf[TX_AU_RINGBUFF_SIZE];
static ring_buff_t au_in_ring_buff;
static char au_in_buf[RX_AU_RINGBUFF_SIZE];

#define UART_DMA_BUFF_SIZE 4096
static struct bflb_device_s *at_uart_dev;
static ATTR_NOCACHE_RAM_SECTION uint8_t uart_tx_buffer[UART_DMA_BUFF_SIZE];

static void aiio_uart_isr(int irq, void *arg)
{
    uint32_t intstatus;
    uint8_t recv_len;
    static uint8_t uart_recv_buf[16];

    intstatus = bflb_uart_get_intstatus(at_uart_dev);

    if (intstatus & UART_INTSTS_RX_FIFO)
    {
        while (bflb_uart_rxavailable(at_uart_dev))
        {
            memset(uart_recv_buf, 0x00,sizeof(uart_recv_buf));
            recv_len = bflb_uart_get(at_uart_dev, uart_recv_buf, sizeof(uart_recv_buf));
            vb6824_write_buff(uart_recv_buf, recv_len);
        }
    }

    if (intstatus & UART_INTSTS_RTO)
    {
        while (bflb_uart_rxavailable(at_uart_dev))
        {
            memset(uart_recv_buf, 0x00,sizeof(uart_recv_buf));
            recv_len = bflb_uart_get(at_uart_dev, uart_recv_buf, sizeof(uart_recv_buf));
            vb6824_write_buff(uart_recv_buf, recv_len);
        }
        bflb_uart_int_clear(at_uart_dev, UART_INTCLR_RTO);
    }
    if (intstatus & UART_INTSTS_TX_FIFO)
    {
        bflb_uart_txint_mask(at_uart_dev, true);
    }
}

static volatile int uart_tx_idle;
static void dma0_ch0_isr(void *arg)
{
    uart_tx_idle = 0;
}

static void uart_dma_init()
{
    struct bflb_dma_channel_config_s tx_config = {
        .direction = DMA_MEMORY_TO_PERIPH,
        .src_req = DMA_REQUEST_NONE,
        .dst_req = DMA_REQUEST_UART1_TX,
        .src_addr_inc = DMA_ADDR_INCREMENT_ENABLE,
        .dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE,
        .src_burst_count = DMA_BURST_INCR1,
        .dst_burst_count = DMA_BURST_INCR1,
        .src_width = DMA_DATA_WIDTH_8BIT,
        .dst_width = DMA_DATA_WIDTH_8BIT,
    };

    dma0_ch0 = bflb_device_get_by_name("dma0_ch0");
    bflb_uart_link_txdma(at_uart_dev, true);
    bflb_dma_channel_init(dma0_ch0, &tx_config);
    bflb_dma_channel_irq_attach(dma0_ch0, dma0_ch0_isr, NULL);
}

static void uart_init(void)
{
    struct bflb_device_s *gpio;
    at_uart_dev = bflb_device_get_by_name(DEFAULT_TEST_UART);
    gpio = bflb_device_get_by_name("gpio");
    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000; 
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    cfg.bit_order = UART_LSB_FIRST;

    bflb_gpio_uart_init(gpio, TX_GPIO, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, RX_GPIO, GPIO_UART_FUNC_UART1_RX);

    bflb_uart_init(at_uart_dev, &cfg);

    // bflb_uart_txint_mask(at_uart_dev, false);
    bflb_uart_rxint_mask(at_uart_dev, false);

    uart_dma_init();

    bflb_irq_attach(at_uart_dev->irq_num, aiio_uart_isr, NULL);
    bflb_irq_enable(at_uart_dev->irq_num);
}

static void write_6824_timer_cb(TimerHandle_t xTimer)
{
    static uint8_t out_paly[BUFFER_SAMPLES];

    if ((ring_buff_get_size(&au_out_ring_buff) >= BUFFER_SAMPLES && tx_buff_mng.last_play_size > 0) ||
        (ring_buff_get_size(&au_out_ring_buff) >= TX_PLAY_THRESHOLD && tx_buff_mng.last_play_size == 0))
    {
        ring_buff_pop_data(&au_out_ring_buff, out_paly, BUFFER_SAMPLES);
        tx_buff_mng.last_play_size = BUFFER_SAMPLES;

        vb6824_play_audio(out_paly, BUFFER_SAMPLES);
    }
    else
    {
        // puts("tx no buff\r\n");
        tx_buff_mng.last_play_size = 0;
    }
}

void uart_send_data(char *data, uint16_t len)
{
    static struct bflb_dma_channel_lli_transfer_s tx_transfers[1];
    static struct bflb_dma_channel_lli_pool_s tx_llipool[2];

    while (uart_tx_idle) {
        puts("uart_wait\r\n");
        vTaskDelay(1);
    }

    memcpy(uart_tx_buffer, data, len);
    tx_transfers[0].src_addr = (uint32_t)uart_tx_buffer;
    tx_transfers[0].dst_addr = (uint32_t)DMA_ADDR_UART1_TDR;
    tx_transfers[0].nbytes = len;

    bflb_dma_channel_lli_reload(dma0_ch0, tx_llipool, 2, tx_transfers, 1);

    uart_tx_idle = 1;
    bflb_dma_channel_start(dma0_ch0);
}

static void oai_init_audio_decoder()
{
    int decoder_error = 0;
    opus_decoder = opus_decoder_create(SAMPLE_RATE, 1, &decoder_error);
    if (decoder_error != OPUS_OK)
    {
        printf("Failed to create OPUS decoder");
        return;
    }

    output_buffer = (opus_int16 *)malloc(BUFFER_SAMPLES * sizeof(opus_int16) * 2);
    memset(output_buffer, 0, BUFFER_SAMPLES * sizeof(opus_int16) * 2);
}

void oai_init_audio_capture()
{
    ring_buff_init(&au_out_ring_buff, au_out_buf, TX_AU_RINGBUFF_SIZE);
    ring_buff_init(&au_in_ring_buff, au_in_buf, RX_AU_RINGBUFF_SIZE);
}

static void _write_au(uint16_t *data, uint16_t len)
{
    while (ring_buff_avail(&au_out_ring_buff) < (BUFFER_SAMPLES * 3) + 1) {
        vTaskDelay(1);
    }

    if (RINGBUFF_OK != ring_buff_push_data(&au_out_ring_buff, data, len))
    {
        puts("_write_au push fail\r\n");
    }
}

void media_decode(uint8_t *data, size_t size)
{
    if (play_tone_flag) {
        // puts("play tone...\r\n");
        return;
    }
    int decoded_size =
        opus_decode(opus_decoder, data, size, output_buffer, BUFFER_SAMPLES * 2, 0);

    if (decoded_size > 0)
    {
        _write_au(output_buffer, decoded_size * 2);
    }
}

void media_push_frame(uint8_t *data, uint16_t len)
{
#ifdef ASR_WAKEUP_INTERRUPT
    if (!rec_flag) {
        return ;
    }
#endif
    if (len != VB6824_OPUS_FRAME_SIZE) {
        printf("invalid 6824 opus frame len:%d\r\n", len);
        return ;
    }
    if (RINGBUFF_OK != ring_buff_push_data(&au_in_ring_buff, data, len)) {
        // puts("6824 push fail\r\n");
    }
}

int media_get_frame(uint8_t **frame)
{
    *frame = NULL;

    if (playing_flag && play_tone_id == TONE_ID_ASR_RESP) {
        ring_buff_pop_data(&au_in_ring_buff, opus_frame, VB6824_OPUS_FRAME_SIZE);
        return 0;
    }

    if (ring_buff_get_size(&au_in_ring_buff) == 0)
    {
        return 0;
    }

    ring_buff_pop_data(&au_in_ring_buff, opus_frame, VB6824_OPUS_FRAME_SIZE);
    *frame = opus_frame;
    return VB6824_OPUS_FRAME_SIZE;
}

typedef struct
{
    uint8_t *data;
    size_t len;
} tone_info_t;

static const tone_info_t tone_list[] = {
    {tone_wozai, sizeof(tone_wozai)},
    {tone_weipeiwang, sizeof(tone_weipeiwang)},
    {tone_kaishipeiwang, sizeof(tone_kaishipeiwang)},
    {tone_peiwangshibai, sizeof(tone_peiwangshibai)},
    {tone_peiwangchenggong, sizeof(tone_peiwangchenggong)},
    {tone_lianwangchenggong, sizeof(tone_lianwangchenggong)},
    {tone_wangluoduankai, sizeof(tone_wangluoduankai)},
    {tone_lianwangzhong, sizeof(tone_lianwangzhong)},
    {tone_lianwangshibai, sizeof(tone_lianwangshibai)},
    {tone_yipeiwang, sizeof(tone_yipeiwang)},
    {tone_tuixia, sizeof(tone_tuixia)}
};

static void _media_play_tone(int id)
{
#define OGG_READ_SIZE 4096
    int stream_init = 0;
    size_t read_pos;
    size_t bytes_read;
    size_t total_bytes = 0;
    ogg_sync_state ogg_sync;
    ogg_stream_state ogg_stream;
    ogg_page ogg_page;
    ogg_packet ogg_packet;
    unsigned char *buffer;
    uint8_t *tone_in;

    /* get tone */
    if (id >= TONE_ID_MAX) {
        printf("invalid tone id:%d\r\n", id);
        return;
    }
    tone_in = tone_list[id].data;
    total_bytes = tone_list[id].len;
    if (!tone_in || !total_bytes) {
        printf("invalid tone:%d\r\n", id);
        return;
    }

    read_pos = 0;
    ogg_sync_init(&ogg_sync);

    play_tone_flag = 1;
    while (1) {
        buffer = ogg_sync_buffer(&ogg_sync, OGG_READ_SIZE);
        if (read_pos >= total_bytes) {
            break;
        }
        if (read_pos + OGG_READ_SIZE > total_bytes) {
            bytes_read = total_bytes - read_pos;
        } else {
            bytes_read = OGG_READ_SIZE;
        }
        memcpy(buffer, tone_in + read_pos, bytes_read);
        read_pos += bytes_read;
        ogg_sync_wrote(&ogg_sync, bytes_read);
        while (ogg_sync_pageout(&ogg_sync, &ogg_page)==1) {
            if (stream_init == 0) {
                stream_init = 1;
                ogg_stream_init(&ogg_stream, ogg_page_serialno(&ogg_page));
            }
            if (ogg_stream_pagein(&ogg_stream, &ogg_page)!= 0) {
                printf("Error submitting page to Ogg stream\n");
                break;
            }

            while (ogg_stream_packetout(&ogg_stream, &ogg_packet) == 1) {
                // printf("head:%x\r\n", ogg_packet.packet[0]);
                int decoded_size =
                    opus_decode(opus_decoder, ogg_packet.packet, ogg_packet.bytes, output_buffer, BUFFER_SAMPLES, 0);
                if (decoded_size > 0) {
                    _write_au(output_buffer, decoded_size * 2);
                } else {
                    // printf("ogg_packet.bytes:%d decoded_size:%d\r\n", ogg_packet.bytes, decoded_size);
                    // break;
                }
            }
        }
    }

    play_tone_flag = 0;
    ogg_stream_clear(&ogg_stream);
    ogg_sync_clear(&ogg_sync);
}

static void _au_task(void *param)
{
    while(1) {
        ulTaskNotifyTake(pdFALSE, portMAX_DELAY);
        playing_flag = 1;
        _media_play_tone(play_tone_id);
        playing_flag = 0;
    }

    vTaskDelete(NULL);
}

void media_init()
{
    vb6824_init();
    uart_init();
    vb6824_set_volume(0x1c);
    vb6824_record_stop();

    oai_init_audio_decoder();
    oai_init_audio_capture();

    xTaskCreate(_au_task, "au", (AU_TASK_STACK_SIZE / 4), NULL, 5, &au_thread_handle);
}

void media_start(void)
{
    write_6824_timer = xTimerCreate("write_6824_timer",
        pdMS_TO_TICKS(10),
        pdTRUE,
        0,
        write_6824_timer_cb
    );
    xTimerStart(write_6824_timer, 0);
}

void media_play_tone(int id)
{
#ifdef ASR_WAKEUP_INTERRUPT
    if (id == TONE_ID_ASR_RESP) {
        last_wakeup_tick = xTaskGetTickCount();
        rec_flag = 1;
    }
#endif
    if (playing_flag && id == TONE_ID_ASR_RESP) {
        return;
    }
    play_tone_id = id;
    xTaskNotifyGive(au_thread_handle);
}
#ifdef ASR_WAKEUP_INTERRUPT
void media_start_rec(void)
{
    rec_flag = 1;
}

void media_stop_rec(void)
{
    // if (xTaskGetTickCount() - last_wakeup_tick > 5000) {
        rec_flag = 0;
    // }
}
#endif