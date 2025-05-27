#include "FreeRTOS.h"
#include "task.h"

#include "lwip/tcpip.h"

#include "bl_fw_api.h"
#include "wifi_mgmr_ext.h"
#include "wifi_mgmr.h"

#include "bflb_irq.h"
#include "bflb_uart.h"

#include "bl616_glb.h"
#include "rfparam_adapter.h"

#include "bflb_mtd.h"

#include "board.h"
#include "shell.h"

#include "stoge.h"
#include "volc.h"
#include "media.h"
#include "http_ota.h"
#include "blufi.h"
#include "axk_board.h"

#include "lcd_display.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define fw_version "1.6"    //version  小数类型，只能带一个小数点的浮点数，如 1.3 

struct bflb_device_s *gpio;

#define WIFI_STACK_SIZE  (1536)
#define TASK_PRIORITY_FW (16)

static struct bflb_device_s *uart0;
static TaskHandle_t wifi_fw_task;
static wifi_conf_t conf = {
    .country_code = "CN",
};

static int netcfg_flag;
static char wifi_ssid[WIFI_SSID_LEN + 1];
static char wifi_pwd[WIFI_PASSWORD_LEN + 1];

uint8_t g_wifi_sta_is_connected = 0;

extern void shell_init_with_task(struct bflb_device_s *shell);

int wifi_start_firmware_task(void)
{
    LOG_I("Starting wifi ...\r\n");

    /* enable wifi clock */

    GLB_PER_Clock_UnGate(GLB_AHB_CLOCK_IP_WIFI_PHY | GLB_AHB_CLOCK_IP_WIFI_MAC_PHY | GLB_AHB_CLOCK_IP_WIFI_PLATFORM);
    GLB_AHB_MCU_Software_Reset(GLB_AHB_MCU_SW_WIFI);

    /* Enable wifi irq */

    extern void interrupt0_handler(void);
    bflb_irq_attach(WIFI_IRQn, (irq_callback)interrupt0_handler, NULL);
    bflb_irq_enable(WIFI_IRQn);

    xTaskCreate(wifi_main, (char *)"fw", WIFI_STACK_SIZE, NULL, TASK_PRIORITY_FW, &wifi_fw_task);

    return 0;
}

volatile uint32_t wifi_state = 0;
void wifi_event_handler(uint32_t code)
{
    switch (code) {
        case CODE_WIFI_ON_INIT_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_INIT_DONE\r\n", __func__);
            wifi_mgmr_init(&conf);
        } break;
        case CODE_WIFI_ON_MGMR_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_MGMR_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_SCAN_DONE: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_SCAN_DONE\r\n", __func__);
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            wifi_state = 1;
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            if (netcfg_flag) {
                uint8_t bssid[6];
                wifi_mgmr_sta_get_bssid(bssid);
                blufi_wifi_connected_report(NULL, bssid);
                netcfg_flag = 0;
                netcfg_adv_stop();
                volc_bot_active();

                if (0 != stoge_wifi_set(wifi_ssid, wifi_pwd)) {
                    LOG_E("wificfg save fail\r\n");
                    return ;
                }
            }
            display_wifi_status(wifi_connected_unicode);
            display_tips("连网成功");     
            display_emoij(smile_unicode);
            display_wifi_status(wifi_connected_unicode);       
            media_play_tone(TONE_ID_NETCONN_SUCCESS);
            volc_start();
            g_wifi_sta_is_connected=1;
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            int32_t s_code = 0;

            display_wifi_status(wifi_disconnected_unicode);
            display_tips("网络断开");
            display_emoij(face_frown_unicode);

            s_code = wifiMgmr.wifi_mgmr_stat_info.status_code;
            if (s_code != WLAN_FW_DISCONNECT_BY_USER_WITH_DEAUTH) {
                if (wifi_state) {
                    media_play_tone(TONE_ID_NETCONN_DISCONN);
                }
                else {
                    media_play_tone(TONE_ID_NETCONN_FAIL);
                }
            }
            wifi_state = 0;
            LOG_I("[APP] [EVT] CODE_WIFI_ON_DISCONNECT:%d\r\n", s_code);
            g_wifi_sta_is_connected=0;
        } break;
        default: {
            LOG_I("[APP] [EVT] Unknown code %u \r\n", code);
        }
    }
}

void netcfg_sta_connect(char *ssid, char *pwd)
{
    display_wifi_status(wifi_disconnected_unicode);
    display_tips("连网中...");
    wifi_mgmr_sta_autoconnect_disable();
    strcpy(wifi_ssid, ssid);
    strcpy(wifi_pwd, pwd);
    wifi_sta_connect(ssid, pwd, NULL, NULL, 1, 0, 0, 1);
    media_play_tone(TONE_ID_NETCONN_CONNECTING);
    netcfg_flag = 1;
}

int cmd_set_wifi_config(int argc, char **argv)
{
    if (argc < 3) {
        LOG_E("argc error ,wifi_set_config [ssid] [password]\r\n");
        return -1;
    }
    if (argc > 2) {
        if (!strlen(argv[1]) || !strlen(argv[2])) {
            return -1;
        }
        if (0 != stoge_wifi_set(argv[1], argv[2])) {
            LOG_E("wificfg save fail\r\n");
            return -1;
        }
    }
    media_play_tone(TONE_ID_NETCFG_SUCCESS);
    vTaskDelay(1000);
    GLB_SW_POR_Reset();
    return 0;
}

static void proc_main_entry(void *pvParameters)
{
    int ret;

    media_init();
    media_start();

    ret = stoge_wifi_get(wifi_ssid, wifi_pwd);
    if (ret) {
        LOG_W("wifi uncfg\r\n");
        media_play_tone(TONE_ID_UNNETCFG);
        goto out_label;
    }

    media_play_tone(TONE_ID_WELCOME);
    vTaskDelay(5000);
    display_wifi_status(wifi_disconnected_unicode);
    display_tips("连网中...");
    media_play_tone(TONE_ID_NETCONN_CONNECTING);
    wifi_mgmr_sta_autoconnect_enable();
    wifi_sta_connect(wifi_ssid, wifi_pwd, NULL, NULL, 1, 0, 0, 1);

out_label:
    vTaskDelete(NULL);
}

int app_main(void)
{
    printf("fw version:%s \r\n", fw_version);

    lcd_display_init();

    bflb_mtd_init();
    stoge_init();
    axk_board_init();

    uart0 = bflb_device_get_by_name("uart0");
    shell_init_with_task(uart0);

    if (0 != rfparam_init(0, NULL, 0)) {
        LOG_I("PHY RF init failed!\r\n");
        return 0;
    }

    tcpip_init(NULL, NULL);
    wifi_start_firmware_task();
    xTaskCreate(lvgl_display_task, (char*)"lvgl_display_task", 1024, NULL, 10, NULL);
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 2, NULL);
    xTaskCreate(http_ota_task, (char *)"http_ota_task", 1024, &fw_version, 1, NULL);
    vTaskStartScheduler();

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

SHELL_CMD_EXPORT_ALIAS(cmd_set_wifi_config, wifi_set_config, set wifi);