#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "lvgl.h"
#include "lv_conf.h"
#include "lv_port_disp.h"

#define smile_unicode 0xf118
#define face_meh_blank_unicode 0xf5a4
#define face_frown_unicode 0xf119
#define face_sad_cry_unicode 0xf5b3
#define face_sleep_unicode 0xe38d
#define wifi_connected_unicode true
#define wifi_disconnected_unicode false

LV_IMG_DECLARE(_wifi_connected_alpha_35x28)
LV_FONT_DECLARE(lv_customer_font_SourceHanSerifSC_Regular_20)
LV_FONT_DECLARE(_smile_0_alpha_100x100)
LV_FONT_DECLARE(_face_frown_0_alpha_100x100)
LV_FONT_DECLARE(_sleep_0_alpha_100x100)

void lcd_display_init(void);
void display_wifi_status(bool connected);
void lvgl_display_task(void *arg);
void display_emoij(int emoij_unicode);
void display_font(char *font_str);
void display_tips(char *tips_str);
void set_font_location(int x,int y);

#endif