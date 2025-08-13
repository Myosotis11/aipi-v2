#ifndef __STOGE_H__
#define __STOGE_H__

#define WIFI_SSID_LEN      32
#define WIFI_PASSWORD_LEN  64
#define VOLC_APPID_LEN     64
#define VOLC_ROOMID_LEN    64
#define VOLC_USERID_LEN    64
#define VOLC_BOTID_LEN     64
#define VOLC_TOKEN_LEN     256
#define VOLC_LICENSE_LEN   1024

#define SG_WIFI_SSID_KEY    "_wifi_config_ssid"
#define SG_WIFI_PWD_KEY     "_wifi_config_password"
#define SG_VOLC_APPID_KEY   "volc_appid"
#define SG_VOLC_ROOMID_KEY  "volc_roomid"
#define SG_VOLC_USERID_KEY  "volc_userid"
#define SG_VOLC_TOKEN_KEY   "volc_token"
#define SG_NETCFG           "netcfg_flag"
#define SG_LICENSE          "volc_license"

int stoge_init();
int stoge_wifi_get(char *ssid, char *pwd);
int stoge_wifi_set(const char *ssid, const char *pwd);
int stoge_volc_get(char *appid, char *roomid, char *userid, char *tokenid);
int stoge_volc_set(const char *appid, const char *roomid, const char *userid, const char *tokenid);
int stoge_netcfg_get(void);
int stoge_netcfg_set(int flag);
int stoge_license_get(char *license);
int stoge_license_set(char *license);

#endif