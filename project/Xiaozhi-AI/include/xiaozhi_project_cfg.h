#ifndef _XIAOZHI_PROJECT_CFG_H_
#define _XIAOZHI_PROJECT_CFG_H_

#define fw_version "1.3"    //version  小数类型，只能带一个小数点的浮点数，如 1.3 

#define DEFAULT_PRODUCT_KEY "BL618_XZ"

#define USE_WSS_URL (1)

#if USE_WSS_URL
    #if 1
        //官方服务器
        //websocket地址
        #define DEFAULT_WS_URL "wss://api.tenclass.net:443/xiaozhi/v1/"
        //激活码和OTA请求地址
        #define WEB_SERVER "api.tenclass.net"
        #define WEB_PORT "443"
        #define WEB_URL "https://api.tenclass.net/xiaozhi/ota/"

        #define ENTERPRISE_WEB_URL "https://api.tenclass.net/xiaozhi/ota/activate"        
    #else
        //私有服务器
        //websocket地址
        #define DEFAULT_WS_URL "wss://cloud95.iot-aithings.com:2882/xiaozhi/v1/"
        //激活码和OTA请求地址
        #define WEB_SERVER "cloud95.iot-aithings.com"
        #define WEB_PORT "2882"
        #define WEB_URL "https://cloud95.iot-aithings.com:2882/xiaozhi/ota/"
    #endif
#endif
#define EXIT_IDEL_TIME_MS	(1000*20)	//空闲退出时间

#endif 
