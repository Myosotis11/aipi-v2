#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "lvgl.h"
#include "lv_conf.h"
#include "lv_port_disp.h"

#define wifi_connected_unicode true
#define wifi_disconnected_unicode false

typedef enum {
    neutral=1,
    happy,
    laughing,
    funny,
    sad,
    angry,
    crying,
    loving,
    embarrassed,
    surprised,
    shocked,
    thinking,
    winking,
    cool,
    relaxed,
    delicious,
    kissy,
    confident,
    sleepy,
    silly,
    confused,
};

LV_IMG_DECLARE(_wifi_connected_alpha_35x28)
LV_FONT_DECLARE(lv_customer_font_SourceHanSerifSC_Regular_20)
LV_IMG_DECLARE(_angry_alpha_75x75)
LV_IMG_DECLARE(_confident_alpha_75x75)
LV_IMG_DECLARE(_confused_alpha_75x75)
LV_IMG_DECLARE(_cool_alpha_75x75)
LV_IMG_DECLARE(_crying_alpha_75x75)
LV_IMG_DECLARE(_delicious_alpha_75x75)
LV_IMG_DECLARE(_embarrassed_alpha_75x75)
LV_IMG_DECLARE(_funny_alpha_75x75)
LV_IMG_DECLARE(_happy_alpha_75x75)
LV_IMG_DECLARE(_kissy_alpha_75x75)
LV_IMG_DECLARE(_laughing_alpha_75x75)
LV_IMG_DECLARE(_loving_alpha_75x75)
LV_IMG_DECLARE(_neutral_alpha_75x75)
LV_IMG_DECLARE(_no_internet_alpha_30x30)
LV_IMG_DECLARE(_relaxed_alpha_75x75)
LV_IMG_DECLARE(_robot_120_120_0_alpha_100x100)
LV_IMG_DECLARE(_robot_alpha_100x100)
LV_IMG_DECLARE(_sad_alpha_75x75)
LV_IMG_DECLARE(_shocked_alpha_75x75)
LV_IMG_DECLARE(_silly_alpha_75x75)
LV_IMG_DECLARE(_sleepy_alpha_75x75)
LV_IMG_DECLARE(_surprised_alpha_75x75)
LV_IMG_DECLARE(_thinking_alpha_75x75)
LV_IMG_DECLARE(_winking_alpha_75x75)
LV_IMG_DECLARE(_winking_alpha_75x75)
// LV_IMG_DECLARE(_robot_alpha_75x75)

void lcd_display_init(void);
void display_wifi_status(bool connected);
void lvgl_display_task(void *arg);
void display_emoij(int emoij_unicode);
void display_font(char *font_str);
void display_tips(char *tips_str);
void set_font_location(int x,int y);
void set_emoil_center();

#endif