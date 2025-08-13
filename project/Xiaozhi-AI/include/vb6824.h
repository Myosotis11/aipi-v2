#ifndef __VB6824_H__
#define __VB6824_H__


#include <stdint.h>

#define Header1     0x55       //包头
#define Header2     0xAA       //包头

#define MicRecord  0x2080       //6824 发送麦克风录音音频

#define ASR        0x0180       //6824 发送识别结构的数据格式

#define AudioPlay  0x2081       //6824 接收WIFI发送的音频数据

#define Recordstop 0x0201       //6824 接收WIFI发送的命令控制 6824 停止发送音频

#define VolumeSet  0x0203       //6824 接收WIFI发送的命令控制播放音量

#define OTA_Mode   0x0205       //6824 接收WIFI发送的命令控制进入OTA模式

#define version_cmd 0x0207       //6824 接收WIFI发送的命令控制获取版本

#define version_ack 0x0107       //6824 发送版本信息（唤醒词）

#define factory_test 0x0208       //6824 接收WIFI发送的命令控制进入产测模式

#define factory_energy 0x2088     //产测能量测试

#define Header_len 2            //包头长度

#define data_len   2           //数据长度

#define cmd_len    2            //命令长度

#define OPUS_LENS  40           //OPUS数据包长度

#define AT_CMD_RINGBUFF_SIZE  1024

#define AIIO_AT_CMD_MAX_LEN   512

void vb6824_init(void);

void vb6824_write_buff(uint8_t *data, uint16_t len);

void send_data_to_6824(uint16_t cmd, uint16_t len, char *data);

void vb6824_record_stop(void);

void vb6824_set_volume(uint8_t volume);

void vb6824_play_audio(char *data,uint16_t len);

void vb6824_clear_buff(void);

void vb6824_rst_module();

void vb6824_enter_ota_mode();

int32_t aiio_at_recv_byte(uint8_t *data,int len);

void vb6824_read_version(void);

void reset_vb6824_fifo(void);

void vb6824_enter_factory_mode();

void vb6824_exit_factory_mode();

void read_vb6824_data_task(void* param);

typedef struct {
char WakeWord[20];
char manufacturer[50];
char class_name[30];
char productName[20];
char language[10];
char version[10];
char MD5[33];
}vb6824_fifo_t;

extern vb6824_fifo_t vb6824_fifo;

typedef int (*p_micRecordReportCallbak)(char *data,int len);

void set_record_report_callback(p_micRecordReportCallbak func);
void set_asr_callback(p_micRecordReportCallbak func);

#endif /* __UART_H__ */