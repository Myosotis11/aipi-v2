#include "stoge.h"
#include "easyflash.h"

int stoge_init()
{
    easyflash_init();

    return 0;
}

int stoge_wifi_get(char *ssid, char *pwd)
{
    int len = 0;

    ef_get_env_blob(SG_WIFI_SSID_KEY, ssid, WIFI_SSID_LEN, &len);
    if (len != WIFI_SSID_LEN) {
        return -1;
    }
    ef_get_env_blob(SG_WIFI_PWD_KEY, pwd, WIFI_PASSWORD_LEN, &len);
    if (len != WIFI_PASSWORD_LEN) {
        return -1;
    }

    if (!strlen(ssid) || !strlen(pwd)) {
        return -1;
    }

    return 0;
}

int stoge_wifi_set(const char *ssid, const char *pwd)
{
    if (0 != ef_set_env_blob(SG_WIFI_SSID_KEY, ssid, WIFI_SSID_LEN)) {
        return -1;
    }
    if (0 != ef_set_env_blob(SG_WIFI_PWD_KEY, pwd, WIFI_PASSWORD_LEN)) {
        return -1;
    }

    return 0;
}

int stoge_volc_get(char *appid, char *roomid, char *userid, char *tokenid)
{
    int len = 0;
    int ret;

    ret = ef_get_env_blob(SG_VOLC_APPID_KEY, appid, VOLC_APPID_LEN, &len);
    if (len != VOLC_APPID_LEN) {
        return -1;
    }
    ret = ef_get_env_blob(SG_VOLC_ROOMID_KEY, roomid, VOLC_ROOMID_LEN, &len);
    if (len!= VOLC_ROOMID_LEN) {
        return -1;
    }
    ret = ef_get_env_blob(SG_VOLC_USERID_KEY, userid, VOLC_USERID_LEN, &len);
    if (len!= VOLC_USERID_LEN) {
        return -1;
    }
    ret = ef_get_env_blob(SG_VOLC_TOKEN_KEY, tokenid, VOLC_TOKEN_LEN, &len);
    if (len!= VOLC_TOKEN_LEN) {
        return -1;
    }

    return 0;
}

int stoge_volc_set(const char *appid, const char *roomid, const char *userid, const char *tokenid)
{
    int len = 0;

    if (0 != ef_set_env_blob(SG_VOLC_APPID_KEY, appid, VOLC_APPID_LEN)) {
        return -1;
    }
    if (0 != ef_set_env_blob(SG_VOLC_ROOMID_KEY, roomid, VOLC_ROOMID_LEN)) {
        return -1;
    }
    if (0 != ef_set_env_blob(SG_VOLC_USERID_KEY, userid, VOLC_USERID_LEN)) {
        return -1;
    }
    if (0 != ef_set_env_blob(SG_VOLC_TOKEN_KEY, tokenid, VOLC_TOKEN_LEN)) {
        return -1;
    }

    return 0;
}

int stoge_netcfg_get(void)
{
    int ret;
    int flag;
    int len = 0;

    ret = ef_get_env_blob(SG_NETCFG, &flag, sizeof(int), &len);
    if (len != sizeof(int)) {
        return -1;
    }
    return 0;
}
int stoge_netcfg_set(int flag)
{
    if (!flag) {
        ef_del_env(SG_NETCFG);
        return 0;
    } else {
        ef_set_env_blob(SG_NETCFG, 1, sizeof(int));
    }

    return 0;
}

int stoge_license_get(char *license)
{
    int len = 0;
    int ret;

    ret = ef_get_env_blob(SG_LICENSE, license, VOLC_LICENSE_LEN, &len);
    if (ret <= 0) {
        return -1;
    }
    license[len] = 0;

    return 0;
}
int stoge_license_set(char *license)
{
    if (!license) {
        ef_del_env(SG_LICENSE);
    }
    if (strlen(license) > VOLC_LICENSE_LEN) {
        return -1;
    }
    if (0 != ef_set_env_blob(SG_LICENSE, license, strlen(license))) {
        return -1;
    }

    return 0;
}