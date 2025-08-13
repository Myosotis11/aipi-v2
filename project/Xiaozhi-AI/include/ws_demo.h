#ifndef __WS_DEMO_H__
#define __WS_DEMO_H__

#include "xiaozhi_api.h"

extern bool USE_VAD;
int mic_record_report_callback(char *data,int len);
void start_listen();
int xiaozhi_active(active_info_t *activeInfo);
#endif 