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
#include "cJSON.h"

extern TaskHandle_t websocketMainTaskHandle;
extern uint32_t last_connect_play_systick;
extern uint8_t g_wifi_sta_is_connected;
uint8_t blufi_start=0; 
extern TimerHandle_t exitTimer;
extern bool enableAudio;

static struct bflb_device_s *gpio;
#define GPIO_LED_PIN 20

void mcp_device_init(void)
{
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_LED_PIN, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
}

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
                vTaskDelay(50 / portTICK_RATE_MS);
                enableAudio=true;
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
        delete_xiaozhi_task();
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

void mcp_message_process(char *input_str,char **output_str)
{
    int id=1;
    int error_code;
    char error_msg[100]={0};
    int ret=0;

    cJSON *root_obj = cJSON_Parse(input_str);
    cJSON *payload_0 = cJSON_GetObjectItem(root_obj, "payload");
    if(NULL==payload_0){
        printf("[%s()-%d]failed to get payload\r\n",__func__,__LINE__);
        ret=-2;
        goto exit2;
    }
    cJSON *ID = cJSON_GetObjectItem(payload_0, "ID");
    if(NULL!=ID){
        id=ID->valueint;    
    }
    
    cJSON *method = cJSON_GetObjectItem(payload_0, "method");
    if(NULL==method){
        printf("[%s()-%d]failed to get method\r\n",__func__,__LINE__);
        ret=-2;
        goto exit2;
    }
    cJSON *root = cJSON_CreateObject();
    if(strcmp(method->valuestring,"initialize")==0)
    {
        // 添加type字段
        cJSON_AddStringToObject(root, "type", "mcp");
        
        // 创建并添加payload对象
        cJSON *payload = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "payload", payload);
        
        // 添加JSON-RPC版本和ID到payload
        cJSON_AddStringToObject(payload, "jsonrpc", "2.0");
        
        cJSON_AddNumberToObject(payload, "id", id);
        
        // 创建并添加result对象到payload
        cJSON *result = cJSON_CreateObject();
        cJSON_AddItemToObject(payload, "result", result);
        
        // 添加protocolVersion到result
        cJSON_AddStringToObject(result, "protocolVersion", "2024-11-05");
        
        // 创建并添加capabilities对象到result
        cJSON *capabilities = cJSON_CreateObject();
        cJSON_AddItemToObject(result, "capabilities", capabilities);
        
        // 创建并添加空的tools对象到capabilities
        cJSON *tools = cJSON_CreateObject();
        cJSON_AddItemToObject(capabilities, "tools", tools);
        
        // 创建并添加serverInfo对象到result
        cJSON *serverInfo = cJSON_CreateObject();
        cJSON_AddStringToObject(serverInfo, "name", "AiPi-PalChatV2");
        cJSON_AddStringToObject(serverInfo, "version", fw_version);
        cJSON_AddItemToObject(result, "serverInfo", serverInfo);
    }
    else if(strcmp(method->valuestring,"notifications/initialized")==0)
    {

    }
    else if(strcmp(method->valuestring,"tools/list")==0)
    {
        cJSON_AddStringToObject(root, "type", "mcp");        
        // 创建payload对象    
        cJSON *payload = cJSON_CreateObject();    
        cJSON_AddItemToObject(root, "payload", payload);        
        // 添加JSON-RPC信息    
        cJSON_AddStringToObject(payload, "jsonrpc", "2.0");    
        cJSON_AddNumberToObject(payload, "id", id);        
        // 创建result对象    
        cJSON *result = cJSON_CreateObject();    
        cJSON_AddItemToObject(payload, "result", result);        
        // 创建tools数组    
        cJSON *tools = cJSON_CreateArray();    
        cJSON_AddItemToObject(result, "tools", tools);        
        // 添加Speaker工具    
        cJSON *speaker = cJSON_CreateObject();    
        cJSON_AddStringToObject(speaker, "name", "Speaker");    
        cJSON_AddStringToObject(speaker, "description", "扬声器");        
        // 创建Speaker的inputSchema    
        cJSON *speakerInputSchema = cJSON_CreateObject();        
        // 创建Speaker的properties对象    
        cJSON *speakerProperties = cJSON_CreateObject();    
        cJSON *volumeProp = cJSON_CreateObject();    
        cJSON_AddStringToObject(volumeProp, "description", "当前音量值");    
        cJSON_AddStringToObject(volumeProp, "type", "number");    
        cJSON_AddItemToObject(speakerProperties, "volume", volumeProp);    
        cJSON_AddItemToObject(speakerInputSchema, "properties", speakerProperties);        
        // 创建Speaker的methods对象    
        cJSON *speakerMethods = cJSON_CreateObject();    
        cJSON *setVolume = cJSON_CreateObject();    
        cJSON_AddStringToObject(setVolume, "description", "设置音量");        
        // 创建SetVolume的parameters    
        cJSON *setVolumeParams = cJSON_CreateObject();    
        cJSON *volumeParam = cJSON_CreateObject();    
        cJSON_AddStringToObject(volumeParam, "description", "0到100之间的整数");    
        cJSON_AddStringToObject(volumeParam, "type", "number");    
        cJSON_AddItemToObject(setVolumeParams, "volume", volumeParam);    
        cJSON_AddItemToObject(setVolume, "parameters", setVolumeParams);        
        cJSON_AddItemToObject(speakerMethods, "SetVolume", setVolume);    
        cJSON_AddItemToObject(speakerInputSchema, "methods", speakerMethods);        
        cJSON_AddItemToObject(speaker, "inputSchema", speakerInputSchema);    
        cJSON_AddItemToArray(tools, speaker);      

        // 添加Light工具    
        cJSON *Light = cJSON_CreateObject();
        // 添加name字段
        cJSON_AddStringToObject(Light, "name", "Light");
        // 添加description字段
        cJSON_AddStringToObject(Light, "description","set the switch of the Light.");
        // 创建inputSchema对象
        cJSON *inputSchema = cJSON_CreateObject();
        // 为inputSchema添加type字段
        cJSON_AddStringToObject(inputSchema, "type", "object");
        // 创建properties对象
        cJSON *properties = cJSON_CreateObject();
        // 创建enabled对象
        cJSON *enabled = cJSON_CreateObject();
        // 为enabled添加属性
        cJSON_AddStringToObject(enabled, "type", "boolean");
        cJSON_AddBoolToObject(enabled, "open", true);
        cJSON_AddBoolToObject(enabled, "close", false);
        // 将enabled添加到properties
        cJSON_AddItemToObject(properties, "enabled", enabled);
        // 将properties添加到inputSchema
        cJSON_AddItemToObject(inputSchema, "properties", properties);
        // 创建required数组
        cJSON *required = cJSON_CreateArray();
        cJSON_AddItemToArray(required, cJSON_CreateString("enabled"));
        // 将required添加到inputSchema
        cJSON_AddItemToObject(inputSchema, "required", required);
        // 将inputSchema添加到根对象
        cJSON_AddItemToObject(Light, "inputSchema", inputSchema);   
        cJSON_AddItemToArray(tools, Light);    
    }
    else if(strcmp(method->valuestring,"tools/call")==0)
    {
        cJSON *params = cJSON_GetObjectItem(payload_0, "params");
        if(NULL==params){
            printf("[%s()-%d]failed to get params\r\n",__func__,__LINE__);
            error_code=-32602;
            memcpy(error_msg, "params is null", strlen("params is null"));
            goto is_error;

        }
        cJSON *name = cJSON_GetObjectItem(params, "name");
        if(NULL==name){
            printf("[%s()-%d]failed to get name\r\n",__func__,__LINE__);
            error_code=-32602;
            memcpy(error_msg, "failed to get name", strlen("failed to get name"));
            goto is_error;
        }
        if(0==strcmp("Speaker",name->valuestring)){
            cJSON *arguments = cJSON_GetObjectItem(params, "arguments");
            if(NULL==arguments){
                printf("[%s()-%d]failed to get arguments\r\n",__func__,__LINE__);
                error_code=-32602;
                memcpy(error_msg, "failed to get arguments", strlen("failed to get arguments"));
                goto is_error;
            }
            cJSON *volume = cJSON_GetObjectItem(arguments, "volume");
            if(NULL==volume){
                printf("[%s()-%d]failed to get volume\r\n",__func__,__LINE__);
                error_code=-32602;
                memcpy(error_msg, "failed to get volume", strlen("failed to get volume"));
                goto is_error;
            }
            if(cJSON_Number!=volume->type){
                printf("[%s()-%d]volume type(%d) error\r\n",__func__,__LINE__,volume->type);
                error_code=-32602;
                memcpy(error_msg, "volume type error", strlen("volume type errore"));
                goto is_error;
            }
            uint8_t vb_volume_value=volume->valueint*VB6824_MAX_VOLUME/100;
            if(vb_volume_value<VB6824_MIN_VOLUME){
                vb_volume_value=VB6824_MIN_VOLUME;
            }
            printf("[%s()-%d]cloud volume:%d set volume:%d\r\n",__func__,__LINE__,volume->valueint,vb_volume_value);
            vb6824_set_volume(vb_volume_value);
            stoge_volume_set(vb_volume_value);
            goto is_success;
        }else if(0==strcmp("Light",name->valuestring)){
            cJSON *arguments = cJSON_GetObjectItem(params, "arguments");
            if(NULL==arguments){
                printf("[%s()-%d]failed to get arguments\r\n",__func__,__LINE__);
                error_code=-32602;
                memcpy(error_msg, "failed to get arguments", strlen("failed to get arguments"));
                goto is_error;
            }
           
            int enabled = -1;

            /* 返回json结构示例：
            {"type":"mcp","payload":{"jsonrpc":"2.0","method":"tools/call","id":4,"params":{"name":"Light","arguments":{"SetEnabled":{"enabled":true}}}},"session_id":"10763747"}
            {"type":"mcp","payload":{"jsonrpc":"2.0","method":"tools/call","id":3,"params":{"name":"Light","arguments":{"method":"SetEnabled","parameters":{"enabled":true}}}},"session_id":"ac388894"}
            {"type":"mcp","payload":{"jsonrpc":"2.0","method":"tools/call","id":3,"params":{"name":"Light","arguments":{"methods":{"SetEnabled":{"enabled":true}}}}},"session_id":"a7d90277"}
            {"type":"mcp","payload":{"jsonrpc":"2.0","method":"tools/call","id":3,"params":{"name":"Light","arguments":{"enabled":true}}},"session_id":"55990641"}    
            */     

            // 尝试结构1: arguments -> SetEnabled -> enabled
            cJSON *set_enabled = cJSON_GetObjectItemCaseSensitive(arguments, "SetEnabled");
            if (cJSON_IsObject(set_enabled)) {
                cJSON *enabled_item = cJSON_GetObjectItemCaseSensitive(set_enabled, "enabled");
                if (cJSON_IsBool(enabled_item)) {
                    enabled = cJSON_IsTrue(enabled_item) ? 1 : 0;
                }
            }
            
            // 尝试结构2: arguments -> method:"SetEnabled" -> parameters -> enabled
            if (enabled == -1) {
                cJSON *method = cJSON_GetObjectItemCaseSensitive(arguments, "method");
                if (cJSON_IsString(method) && strcmp(method->valuestring, "SetEnabled") == 0) {
                    cJSON *parameters = cJSON_GetObjectItemCaseSensitive(arguments, "parameters");
                    if (cJSON_IsObject(parameters)) {
                        cJSON *enabled_item = cJSON_GetObjectItemCaseSensitive(parameters, "enabled");
                        if (cJSON_IsBool(enabled_item)) {
                            enabled = cJSON_IsTrue(enabled_item) ? 1 : 0;
                        }
                    }
                }
            }
            
            // 尝试结构3: arguments -> methods -> SetEnabled -> enabled
            if (enabled == -1) {
                cJSON *methods = cJSON_GetObjectItemCaseSensitive(arguments, "methods");
                if (cJSON_IsObject(methods)) {
                    cJSON *set_enabled = cJSON_GetObjectItemCaseSensitive(methods, "SetEnabled");
                    if (cJSON_IsObject(set_enabled)) {
                        cJSON *enabled_item = cJSON_GetObjectItemCaseSensitive(set_enabled, "enabled");
                        if (cJSON_IsBool(enabled_item)) {
                            enabled = cJSON_IsTrue(enabled_item) ? 1 : 0;
                        }
                    }
                }
            }
            
            // 尝试结构4: arguments -> enabled
            if (enabled == -1) {
                cJSON *enabled_item = cJSON_GetObjectItemCaseSensitive(arguments, "enabled");
                if (cJSON_IsBool(enabled_item)) {
                    enabled = cJSON_IsTrue(enabled_item) ? 1 : 0;
                }
            }

            if(enabled!= -1)
            {
                printf("[%s()-%d]Light enabled:%d\r\n",__func__,__LINE__,enabled);
                if (enabled){
                    bflb_gpio_set(gpio, GPIO_LED_PIN);
                }else{
                    bflb_gpio_reset(gpio, GPIO_LED_PIN);
                }
            }
            else
            {
                printf("[%s()-%d]failed to get enabled\r\n",__func__,__LINE__);
                error_code=-32602;
                memcpy(error_msg, "failed to get enabled", strlen("failed to get enabled"));
                goto is_error;                
            }
            
            goto is_success;
            
        }

is_error:
        // 添加type字段
        cJSON_AddStringToObject(root, "type", "mcp");
        
        // 创建并添加payload对象
        cJSON *payload_err = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "payload", payload_err);
        
        // 添加JSON-RPC版本和ID到payload
        cJSON_AddStringToObject(payload_err, "jsonrpc", "2.0");
        cJSON_AddNumberToObject(payload_err, "id", id);
        
        // 创建并添加error对象到payload
        cJSON *error = cJSON_CreateObject();
        cJSON_AddItemToObject(payload_err, "error", error);
        
        // 添加错误码和错误消息
        cJSON_AddNumberToObject(error, "code", error_code);
        cJSON_AddStringToObject(error, "message", error_msg);       
        goto ACK;  

is_success:
        // 添加type字段
        cJSON_AddStringToObject(root, "type", "mcp");
        
        // 创建并添加payload对象
        cJSON *payload_suc = cJSON_CreateObject();
        cJSON_AddItemToObject(root, "payload", payload_suc);
        
        // 添加JSON-RPC版本和ID到payload
        cJSON_AddStringToObject(payload_suc, "jsonrpc", "2.0");
        cJSON_AddNumberToObject(payload_suc, "id", id);
        
        // 创建并添加result对象到payload
        cJSON *result = cJSON_CreateObject();
        cJSON_AddItemToObject(payload_suc, "result", result);
        
        // 创建content数组
        cJSON *content = cJSON_CreateArray();
        cJSON_AddItemToObject(result, "content", content);
        
        // 创建text类型的内容项
        cJSON *textItem = cJSON_CreateObject();
        cJSON_AddStringToObject(textItem, "type", "text");
        cJSON_AddStringToObject(textItem, "text", "true");
        cJSON_AddItemToArray(content, textItem);
        
        // 添加isError字段
        cJSON_AddBoolToObject(result, "isError", false); // 0表示false    
        
        goto ACK;
    }
    
ACK:            
    // 生成JSON字符串
    *output_str = cJSON_Print(root);
    if (*output_str == NULL) {  // 检查是否生成成功
        printf("[%s()-%d]json_str is null\r\n", __func__, __LINE__);
        cJSON_Delete(root);
        goto exit1;
    }

exit1:
    cJSON_Delete(root);     
exit2:
    cJSON_Delete(root_obj);     
    // printf("[%s()-%d]json_str:%s\r\n",__func__,__LINE__,*output_str);
}