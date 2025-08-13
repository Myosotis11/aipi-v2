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
#include "ws_demo.h"
#include "xiaozhi_project_cfg.h"
#include "lcd_display.h"
#include "xiaozhi_api.h"

extern TaskHandle_t websocketMainTaskHandle;
extern uint32_t last_connect_play_systick;
extern uint8_t g_wifi_sta_is_connected;
uint8_t blufi_start=0; 
extern TimerHandle_t exitTimer;
extern bool enableAudio;

const tone_info_t tone_list[24] = {
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
    {tone_tuixia, sizeof(tone_tuixia)},
    {tone_auth_code, sizeof(tone_auth_code)},
    {tone_zero, sizeof(tone_zero)},
    {tone_one, sizeof(tone_one)},
    {tone_two, sizeof(tone_two)},
    {tone_three, sizeof(tone_three)},
    {tone_four, sizeof(tone_four)},
    {tone_five, sizeof(tone_five)},
    {tone_six, sizeof(tone_six)},
    {tone_seven, sizeof(tone_seven)},
    {tone_eight, sizeof(tone_eight)},
    {tone_nine, sizeof(tone_nine)},      
    {tone_talk_interrupt,sizeof(tone_talk_interrupt)},
    {tone_wakeUp_interrupt,sizeof(tone_wakeUp_interrupt)} ,
};

int vb6824_asr_data_process(char *data,int len){
    // printf("[%s()-%d]\r\nxPortGetFreeHeapSize:%d\r\n",__func__,__LINE__,aiio_os_get_free_heap_size());
    printf("ASR\r\n");
    for(int i=0;i<len;i++){
        printf("%02x ",data[i]);
    }
    printf("\r\n");

    if(0==memcmp("你好小安",data,strlen("你好小安")))
    {
        printf("[%s()-%d]wake up\r\n",__func__,__LINE__);
        if(ACTIVE_CODE_MODE_ACTIVATED!=active_state){
            // set_sys_voic_index(1);
            media_play_tone(TONE_ID_ASR_RESP);
            printf("[%s()-%d]The device has not been activated yet, not responding\r\n",__func__,__LINE__);
            return 0;
        }
        // wifi_mgmr_sta_state_get(&wifi_state);
        // if (wifi_state == WIFI_STATE_CONNECTED_IP_GOT){
        if(g_wifi_sta_is_connected){
            printf("[%s()-%d]wifi connected\r\n",__func__,__LINE__);
            if (exitTimer != NULL) {
                // 刷新定时器
                if (xTimerChangePeriod(exitTimer, pdMS_TO_TICKS(EXIT_IDEL_TIME_MS), 0) == pdPASS) {
                    printf("[%s()-%d]Timer period changed to %dms\r\n",__func__,__LINE__,EXIT_IDEL_TIME_MS);
                } else {
                    printf("[%s()-%d]Failed to change timer period\r\n",__func__,__LINE__);
                }
            }
            if(websocketMainTaskHandle){
                printf("[%s()-%d]Interrupt the conversation\r\n",__func__,__LINE__);
                enableAudio=false;
                send_interrupt_message();
                start_listen();
            }else{
                printf("[%s()-%d]create websocketMainTaskHandle\r\n",__func__,__LINE__);
                ws_test(DEFAULT_WS_URL);
                vTaskDelay(20 / portTICK_RATE_MS);
            }
            display_emoij(happy);
            display_tips("聆听中...");
            media_play_tone(TONE_ID_ASR_RESP);
        }else{
            printf("[%s()-%d]wifi not connected,state=%d\r\n",__func__,__LINE__,g_wifi_sta_is_connected);
            if(blufi_start==0){
                if(xTaskGetTickCount()-last_connect_play_systick>1000*15){
                    media_play_tone(TONE_ID_NETCONN_DISCONN);
                    // last_connect_play_systick=xTaskGetTickCount();
                }else{
                    printf("[%s()-%d]start wifi config,skip voic play\r\n",__func__,__LINE__);
                }
            }
            else
            {
                media_play_tone(TONE_ID_ASR_RESP);
            }
        }
        
    }
    else if(0==memcmp("开始配网",data,strlen("开始配网"))){
        printf("[%s()-%d]start blufi\r\n",__func__,__LINE__);
        media_play_tone(TONE_ID_NETCFG_START);
        wifi_mgmr_sta_autoconnect_disable();
        wifi_sta_disconnect();
        netcfg_start();       
        blufi_start=1;
    }else if(0==memcmp("停止配网",data,strlen("停止配网"))){
        printf("[%s()-%d]stop blufi\r\n",__func__,__LINE__);
        // at_blufi_stop();
        blufi_start=0;
    }
    else if(0==memcmp("打开连续对话模式",data,strlen("打开连续对话模式"))){
        if(websocketMainTaskHandle){
            printf("[%s()-%d]Interrupt the conversation\r\n",__func__,__LINE__);
            send_interrupt_message();
            start_listen();
        }        
        printf("[%s()-%d]turn on continuous conversation mode\r\n",__func__,__LINE__);
        USE_VAD=1;
        media_play_tone(TONE_ID_TALK_INT);
    }else if(0==memcmp("关闭连续对话模式",data,strlen("关闭连续对话模式"))){
        if(websocketMainTaskHandle){
            printf("[%s()-%d]Interrupt the conversation\r\n",__func__,__LINE__);
            send_interrupt_message();
            start_listen();
        }           
        printf("[%s()-%d]turn off continuous conversation mode\r\n",__func__,__LINE__);
        USE_VAD=0;   
         media_play_tone(TONE_ID_WAKEUP_INT);     
    }
    
    return 0;
}