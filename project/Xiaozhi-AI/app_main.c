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
// #include "volc.h"
#include "media.h"
#include "http_ota.h"
#include "blufi.h"
#include "axk_board.h"

#include "lcd_display.h"
#include "vb6824_http_update.h"
#include "xiaozhi_project_cfg.h"
#include "xiaozhi_api.h"

#define DBG_TAG "MAIN"
#include "log.h"

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
uint32_t last_connect_play_systick=0; //记录上次连接播放的时间

active_info_t activeInfo={WEB_SERVER,WEB_PORT,WEB_URL};

TaskHandle_t wifi_module_ota_task_handle = NULL;
TaskHandle_t vb6824_module_ota_task_handle = NULL;

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

void wifi_connected_task(void)
{
    if (netcfg_flag) {
        uint8_t bssid[6];
        wifi_mgmr_sta_get_bssid(bssid);
        blufi_wifi_connected_report(NULL, bssid);
        netcfg_flag = 0;
        netcfg_adv_stop();
        extern ActiveState active_state;
        active_state=ACTIVE_CODE_MODE_INIT;
        xiaozhi_active(&activeInfo);
        if (0 != stoge_wifi_set(wifi_ssid, wifi_pwd)) {
            LOG_E("wificfg save fail\r\n");
        }
    }
    display_wifi_status(wifi_connected_unicode);
    display_tips("连网成功");     
    display_emoij(happy);
    display_wifi_status(wifi_connected_unicode);       
    media_play_tone(TONE_ID_NETCONN_SUCCESS);  
    vTaskDelete(NULL);
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
        case CODE_WIFI_ON_CONNECTING: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTING\r\n", __func__);
            last_connect_play_systick=xTaskGetTickCount();
        } break;
        case CODE_WIFI_ON_CONNECTED: {
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_CONNECTED\r\n", __func__);
        } break;
        case CODE_WIFI_ON_GOT_IP: {
            wifi_state = 1;
            LOG_I("[APP] [EVT] %s, CODE_WIFI_ON_GOT_IP\r\n", __func__);
            xTaskCreate(wifi_connected_task, (char*)"wifi_connected_task", 1024, NULL, 10, NULL);   
            g_wifi_sta_is_connected=1;
        } break;
        case CODE_WIFI_ON_DISCONNECT: {
            int32_t s_code = 0;

            display_wifi_status(wifi_disconnected_unicode);
            display_tips("网络断开");
            display_emoij(sad);

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

static void cmd_factory_mode(char *buf, int len, int argc, char **argv)
{
    vb6824_enter_factory_mode();
}

static void cmd_exit_factory_mode(char *buf, int len, int argc, char **argv)
{
    uint8_t volume = 27;
    vb6824_exit_factory_mode();
    vTaskDelay(1000);
    vb6824_set_volume(volume);
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

int cmd_read_productKey(int argc, char **argv)
{
    char productKey[32]={0};
    int ret=stoge_productKey_get(productKey);
    if(ret!=0)
    {
        printf("read productKey fail,use default productKey\r\n");
        memcpy(productKey,DEFAULT_PRODUCT_KEY,32);
    }
    printf("read OK,productKey:%s\r\n", productKey);
    return 0;
}

int cmd_write_productKey(int argc, char **argv)
{
    if (argc < 2) {
        printf("argc error ,write_productKey [productKey]\r\n");
        return -1;
    }  
    if (argc > 1) {
        if (!strlen(argv[1])) {
            return -1;
        }
        // printf("write productKey:%s\r\n", argv[1]);
        if (0 != stoge_productKey_set(argv[1])) {
            printf("write productKey fail\r\n");
            return -1;
        }
        cmd_read_productKey(NULL, NULL);
    }
    return 0;
}


#define MAX_URL_LENGTH 128
static int server_url_parse(const char *url, char *host, char *port)
{
    const char *start = url;
    const char *colon = NULL;
    int default_port = 443;

    if (!strncmp(url, "http://", 7)) {
        start += 7;
        default_port = 80;
    } else if (!strncmp(url, "https://", 8)) {
        start += 8;
    }

    colon = strchr(start, ':');

    if (colon) {
        strncpy(host, start, colon - start);
        host[colon - start] = '\0';
        strcpy(port, colon + 1);
    } else {
        strcpy(host, start);
        sprintf(port, "%d", default_port);
    }

    return 0;
}

int parse_json_server_url(const char *json, char *url)
{
    const char *key = "\"serverUrl\":";
    const char *start = strstr(json, key);
    if (!start) return -1;

    start += strlen(key);
    while (*start == ' ' || *start == '"') start++;

    const char *end = start;
    while (*end && *end != '"') end++;

    if (end - start >= MAX_URL_LENGTH) return -1;
    strncpy(url, start, end - start);
    url[end - start] = '\0';

    return 0;
}

void netcfg_custom_data_handle(uint8_t *data, int32_t length)
{
    char json[256];
    char srv_url[MAX_URL_LENGTH];
    char host[64];
    char port[16];

    memcpy(json, data, length);
    json[length] = 0;
    LOG_I("netcfg_custom_data_handle:%s\r\n", json);

    /* remove json{} */
    if (parse_json_server_url(json, srv_url) != 0) {
        LOG_E("url parse fail\r\n");
        return ;
    }

    /* parse url */
    if (server_url_parse(srv_url, host, port) != 0) {
        LOG_E("url parse fail\r\n");
        return ;
    }
    LOG_I("parse:%s %s\r\n", host, port);

    // if (stoge_server_hostname_set(host) != 0) {
    //     LOG_E("server hostname save fail\r\n");
    //     return ;
    // }

    // if (stoge_server_port_set(port) != 0) {
    //     LOG_E("server port save fail\r\n");
    //     return ;
    // }
}


int cmd_read_license(int argc, char **argv)
{
    char licenseKey[64]={0};
    char serialNumber[64]={0};
    int ret=stoge_license_get(licenseKey, serialNumber);
    if(ret!=0)
    {
        printf("read license fail\r\n");
        return -1;
    }
    printf("read license OK,licenseKey:%s,serialNumber:%s\r\n", licenseKey,serialNumber);
    return 0;
}

int cmd_write_license(int argc, char **argv)
{
    if (argc < 3) {
        printf("argc error ,write_license [license_key] [serial_number]\r\n");
        return -1;
    }  
    if (argc > 2) {
        if (!strlen(argv[1]) || !strlen(argv[2])) {
            return -1;
        }
        if (0 != stoge_license_set(argv[1],argv[2])) {
            printf("write license fail\r\n");
            return -1;
        }
        cmd_read_license(NULL, NULL);
    }
    return 0;
}

static void proc_main_entry(void *pvParameters)
{
    int ret;
    
    media_init();
    media_start();
    xiaozhi_init(&activeInfo);

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
    printf("********************************************* fw version:%s *********************************************\r\n", fw_version);
    printf("*********************************************compile time:%s %s********************************************\r\n",__DATE__,__TIME__);   
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

    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 2, NULL);
    xTaskCreate(lvgl_display_task, (char*)"lvgl_display_task", 1024, NULL, 10, NULL);
    xTaskCreate(uart_ota_task, (char*)"uart_ota_task", 1024, &fw_version, 5, &vb6824_module_ota_task_handle);
    xTaskCreate(http_ota_task, (char *)"http_ota_task", 1024, &fw_version, 5, &wifi_module_ota_task_handle);
    vTaskStartScheduler();

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

SHELL_CMD_EXPORT_ALIAS(cmd_set_wifi_config, wifi_set_config, set wifi);
SHELL_CMD_EXPORT_ALIAS(cmd_factory_mode, enter_factory_mode, enter factory_mode);
SHELL_CMD_EXPORT_ALIAS(cmd_exit_factory_mode, exit_factory_mode, exit factory_mode);
SHELL_CMD_EXPORT_ALIAS(cmd_read_productKey, read_productKey, read productKey);
SHELL_CMD_EXPORT_ALIAS(cmd_write_productKey, write_productKey, write productKey);
SHELL_CMD_EXPORT_ALIAS(cmd_read_license, read_license, read license);
SHELL_CMD_EXPORT_ALIAS(cmd_write_license, write_license, write license);