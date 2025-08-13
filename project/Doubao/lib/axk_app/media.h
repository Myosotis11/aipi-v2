#ifndef __MEDIA_H__
#define __MEDIA_H__

#define TONE_ID_ASR_RESP           0x00
#define TONE_ID_UNNETCFG           0x01
#define TONE_ID_NETCFG_START       0x02
#define TONE_ID_NETCFG_FAIL        0x03
#define TONE_ID_NETCFG_SUCCESS     0x04
#define TONE_ID_NETCONN_SUCCESS    0x05
#define TONE_ID_NETCONN_DISCONN    0x06
#define TONE_ID_NETCONN_CONNECTING 0x07
#define TONE_ID_NETCONN_FAIL       0x08
#define TONE_ID_WELCOME            0x09
#define TONE_ID_STOP               0x0A
#define TONE_ID_MAX                0x0B

void media_init();
void media_decode(uint8_t *data, size_t size);
int media_get_frame(uint8_t **frame);
void media_start(void);
void media_play_tone(int id);
void media_start_rec(void);
void media_stop_rec(void);
void media_push_frame(uint8_t *data, uint16_t len);

#endif