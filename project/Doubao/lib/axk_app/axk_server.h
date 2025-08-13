#ifndef __AXK_SERVER_H__
#define __AXK_SERVER_H__

int axk_server_request(char *app_id, char *room_id, char *user_id, char *bot_id, char *access_token);
int axk_server_active(void);
int axk_license_request(char *license);

#endif