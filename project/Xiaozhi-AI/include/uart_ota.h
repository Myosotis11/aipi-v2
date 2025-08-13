
#ifndef __UART_OTA_H__
#define __UART_OTA_H__

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "vb6824.h"

#define MSG_UART_UPDATE_READY       0x1
#define MSG_UART_UPDATE_START       0x2
#define MSG_UART_UPDATE_START_RSP   0X3
#define MSG_UART_UPDATE_READ_RSP    0x4

#define PROTOCAL_SIZE       528
#define SYNC_SIZE           6
#define SYNC_MARK0          0xAA
#define SYNC_MARK1          0x55

typedef union {
    uint8_t raw_data[PROTOCAL_SIZE + SYNC_SIZE];
    struct {
        uint8_t mark0;
        uint8_t mark1;
        uint16_t length;
        uint8_t data[PROTOCAL_SIZE + 2]; //最后CRC16
    } data;
} protocal_frame_t;

struct file_info {
    uint8_t cmd;
    uint32_t addr;
    uint32_t len;
} __attribute__((packed));

typedef struct __update_io {
    uint16_t rx;
    uint16_t tx;
    uint8_t  input_channel;                  //input channel选择，根据方案选择未被使用的channel
    uint8_t  output_channel;                 //同input channel
} uart_update_cfg;

typedef enum {
    vb_uart_update_state_idle,
    vb_uart_update_state_http_download,
    vb_uart_update_state_start,
    vb_uart_update_state_success,
    vb_uart_update_state_error,
}vb_uart_update_state_t;

extern vb_uart_update_state_t vb_uart_update_state;
// void uart_ota_task(void *param);
int uart_update_init();

#endif /* __UART_OTA_H__ */