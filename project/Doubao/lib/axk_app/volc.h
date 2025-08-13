#ifndef __VOLC_H__
#define __VOLC_H__

int volc_start(void);
int volc_stop(void);
int volc_key_init(void);
int volc_bot_interrupt(void);
int volc_bot_active(void);

int volc_user_info_handle(char *info, int32_t length);

#endif