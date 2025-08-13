#ifndef __FS_H__
#define __FS_H__

#include "stdint.h"

#define FS_BIN_NAME      "/romfs/vb6824_voice.ufw"

int fs_bin_read(uint8_t *buff, uint32_t offset, uint16_t buff_size);
int fs_bin_write(int dst_offset, const void *src, int size);
int fs_bin_erase(int start_addr, int size);

int init_ota_partition(void);

#endif /* __FS_H__ */