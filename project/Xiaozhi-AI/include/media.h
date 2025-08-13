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
#define TONE_ID_AUTH_CODE          0x0B
#define TONE_ID_ZERO               0x0C
#define TONE_ID_ONE                0x0D
#define TONE_ID_TWO                0x0E
#define TONE_ID_THREE              0x0F
#define TONE_ID_FOUR               0x10
#define TONE_ID_FIVE               0x11
#define TONE_ID_SIX                0x12
#define TONE_ID_SEVEN              0x13
#define TONE_ID_EIGHT              0x14
#define TONE_ID_NINE               0x15
#define TONE_ID_TALK_INT           0x16
#define TONE_ID_WAKEUP_INT         0x17
#define TONE_ID_MAX                0x18


#define VB6824_MAX_VOLUME   (31)    //VB最大音量
#define VB6824_MIN_VOLUME   (5)     //VB最大音量

void media_init();
void media_decode(uint8_t *data, size_t size);
void media_start(void);
void media_play_tone(int id);
void media_start_rec(void);
void media_stop_rec(void);
void uart_set_baudrate(uint32_t baudrate);

extern bool audio_enable;    // audio enable flag

#endif