#include "lcd_display.h"
#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>

#include "gui_guider.h"
#include "custom.h"

lv_ui guider_ui;

void lcd_display_init(void)
{
    lv_init();

    lv_port_disp_init();

    setup_ui(&guider_ui);
    lv_obj_set_style_text_font(guider_ui.screen_tips_label, &lv_customer_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(guider_ui.screen_font_label, &lv_customer_font_SourceHanSerifSC_Regular_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_align(guider_ui.screen_font_label, LV_ALIGN_CENTER, 35, 100);
}

void display_emoij(int emoij_unicode)
{
    switch(emoij_unicode)
    {
        case smile_unicode:
            lv_img_set_src(guider_ui.screen_emoij_label, &_smile_0_alpha_100x100);
            break;
        case face_frown_unicode:
            lv_img_set_src(guider_ui.screen_emoij_label, &_face_frown_0_alpha_100x100);       
            break;
        case face_sleep_unicode:
            lv_img_set_src(guider_ui.screen_emoij_label, &_sleep_0_alpha_100x100);    
            break;
    }
    display_font(" ");   
    lv_obj_set_pos(guider_ui.screen_emoij_label, 70, 110);
}

void set_emoil_center()
{
    lv_obj_set_pos(guider_ui.screen_emoij_label, 70, 110);
}

void set_emoij_left()
{
    lv_obj_set_pos(guider_ui.screen_emoij_label, -10, 110);
}

void display_font(char *font_str)
{
    lv_label_set_text(guider_ui.screen_font_label, font_str);
    set_emoij_left();
}

void set_font_location(int x,int y)
{
    lv_obj_align(guider_ui.screen_font_label, LV_ALIGN_CENTER, x, y);
}

void display_tips(char *tips_str)
{
    lv_label_set_text(guider_ui.screen_tips_label, tips_str);
}

void display_wifi_status(bool connected)
{
    if(connected)
    {
        lv_img_set_src(guider_ui.screen_wifi_label, &_wifi_connected_alpha_35x28);
        lv_obj_set_size(guider_ui.screen_wifi_label, 35, 28);        
    }
    else
    {
        lv_img_set_src(guider_ui.screen_wifi_label, &_no_internet_alpha_30x30);
        lv_obj_set_size(guider_ui.screen_wifi_label, 30, 30);        
    }
}

void lvgl_display_task(void *arg)
{
    int scroll_pos = 0;
    while(1)
    {
        lv_task_handler();
        vTaskDelay(30 / portTICK_RATE_MS);
    }
}