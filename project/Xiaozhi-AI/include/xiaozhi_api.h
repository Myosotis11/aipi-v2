#ifndef  __XIAO_ZHI_API_H__
#define __XIAO_ZHI_API_H__

#include<stdint.h>
#include "stdio.h"

//激活码状态
typedef enum {
    ACTIVE_CODE_MODE_INIT,  //初始化状态 
    ACTIVE_CODE_MODE_ACTIVATED,       //已经激活
    ACTIVE_CODE_MODE_ACTIVATING,    //获取到激活码，正在激活中
    ACTIVE_CODE_MODE_ACTIVATION_FAIL    //激活失败
} ActiveState;

extern ActiveState active_state;

typedef struct __active_info_t{
    char *server;
    char *portStr;
    char *url;
}active_info_t;

typedef struct
{
    uint8_t *data;
    size_t len;
} tone_info_t;

int vb6824_asr_data_process(char *data,int len);
int vb6824_asr_data_process(char *data,int len);

#endif // ! __XIAO_ZHI_API_H__
