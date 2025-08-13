#ifndef __BLUFI_H__
#define __BLUFI_H__

#include <stdint.h>

typedef void (*axk_ble_cb)(struct bt_conn *conn, uint8_t reason);

int netcfg_start(void);
int netcfg_adv_stop(void);
int hal_ble_gatt_blufi_notify_send(uint8_t *buf, uint16_t len);

void blufi_wifi_connected_report(char *ssid, uint8_t *bssid);
void netcfg_sta_connect(char *ssid, char *pwd);
void netcfg_custom_data_handle(uint8_t *data, int32_t length);
int axk_ble_register_event_cb(axk_ble_cb connect_cb,axk_ble_cb disconnect_cb);

#endif /* __BLUFI_H__ */
