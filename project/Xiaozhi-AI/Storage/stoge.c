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

int stoge_volume_set(uint8_t volume)
{
    if (0 != ef_set_env_blob(volume_key, &volume, sizeof(uint8_t))) {
        return -1;
    }    
    return 0;
}

int stoge_volume_get(uint8_t *volume)
{
    int len = 0;
    if (0 != ef_get_env_blob(volume_key, volume, sizeof(uint8_t), &len)) {
        return -1;
    }
    return 0;
}

int stoge_productKey_set(const char *productKey)
{
    if (0 != ef_set_env_blob(product_key, productKey, PRODUCT_KEY_LEN)) {
        return -1;
    }   
    return 0;
}

int stoge_productKey_get(char *productKey)
{
    int len = 0;

    ef_get_env_blob(product_key, productKey, PRODUCT_KEY_LEN, &len);
    if (len != PRODUCT_KEY_LEN) {   
        return -1;
    }

    return 0;
}

int stoge_server_hostname_get(char *hostname)
{
    int len = 0;
    int ret;

    ret = ef_get_env_blob(SG_SERVER_HOST, hostname, VOLC_HOSTNAME_LEN, &len);
    if (ret <= 0) {
        return -1;
    }
    hostname[len] = 0;

    return 0;
}
int stoge_server_hostname_set(char *hostname)
{
    if (!hostname) {
        ef_del_env(SG_SERVER_HOST);
    }
    if (strlen(hostname) > VOLC_HOSTNAME_LEN) {
        return -1;
    }
    if (0 != ef_set_env_blob(SG_SERVER_HOST, hostname, strlen(hostname))) {
        return -1;
    }

    return 0;
}

int stoge_server_port_get(char *port)
{
    int len = 0;
    int ret;

    ret = ef_get_env_blob(SG_SERVER_PORT, port, VOLC_PORT_LEN, &len);
    if (ret <= 0) {
        return -1;
    }
    port[len] = 0;

    return 0;
}
int stoge_server_port_set(char *port)
{
    if (!port) {
        ef_del_env(SG_SERVER_PORT);
    }
    if (strlen(port) > VOLC_PORT_LEN) {
        return -1;
    }
    if (0 != ef_set_env_blob(SG_SERVER_PORT, port, strlen(port))) {
        return -1;
    }

    return 0;
}

int stoge_license_get(char *licenseKey, char *serialNumber)
{
    int len = 0;

    ef_get_env_blob(_license_key, licenseKey, license_key_len, &len);
    if (len != license_key_len) {
        return -1;
    }
    ef_get_env_blob(_serial_number, serialNumber, license_key_len, &len);
    if (len != license_key_len) {
        return -1;
    }

    if (!strlen(licenseKey) || !strlen(serialNumber)) { 
        return -1;
    }

    return 0;
}

int stoge_license_set( char *licenseKey,  char *serialNumber)
{
    if (0 != ef_set_env_blob(_license_key, licenseKey, license_key_len)) {
        return -1;
    }
    if (0 != ef_set_env_blob(_serial_number, serialNumber, license_key_len)) {
        return -1;
    }

    return 0;
}