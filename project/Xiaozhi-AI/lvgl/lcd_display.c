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
        case neutral:
            lv_img_set_src(guider_ui.screen_emoij_label, &_neutral_alpha_75x75);
            break;
        case happy:
            lv_img_set_src(guider_ui.screen_emoij_label, &_happy_alpha_75x75);       
            break;
        case laughing:
            lv_img_set_src(guider_ui.screen_emoij_label, &_laughing_alpha_75x75);    
            break;
        case funny:
            lv_img_set_src(guider_ui.screen_emoij_label, &_funny_alpha_75x75);    
            break;
        case sad:
            lv_img_set_src(guider_ui.screen_emoij_label, &_sad_alpha_75x75);        
            break;
        case angry:
            lv_img_set_src(guider_ui.screen_emoij_label, &_angry_alpha_75x75);     
            break;
        case crying:
            lv_img_set_src(guider_ui.screen_emoij_label, &_crying_alpha_75x75);    
            break;
        case loving:
            lv_img_set_src(guider_ui.screen_emoij_label, &_loving_alpha_75x75);    
            break;
        case embarrassed:
            lv_img_set_src(guider_ui.screen_emoij_label, &_embarrassed_alpha_75x75);    
            break;
        case surprised:
            lv_img_set_src(guider_ui.screen_emoij_label, &_surprised_alpha_75x75);    
            break;
        case shocked:
            lv_img_set_src(guider_ui.screen_emoij_label, &_shocked_alpha_75x75);    
            break;
        case thinking:
            lv_img_set_src(guider_ui.screen_emoij_label, &_thinking_alpha_75x75);    
            break;
        case winking:
            lv_img_set_src(guider_ui.screen_emoij_label, &_winking_alpha_75x75);    
            break;
        case cool:
            lv_img_set_src(guider_ui.screen_emoij_label, &_cool_alpha_75x75);    
            break;
        case relaxed:
            lv_img_set_src(guider_ui.screen_emoij_label, &_relaxed_alpha_75x75);    
            break;
        case delicious:
            lv_img_set_src(guider_ui.screen_emoij_label, &_delicious_alpha_75x75);    
            break;
        case kissy:
            lv_img_set_src(guider_ui.screen_emoij_label, &_kissy_alpha_75x75);    
            break;
        case confident:
            lv_img_set_src(guider_ui.screen_emoij_label, &_confident_alpha_75x75);    
            break;
        case sleepy:
            lv_img_set_src(guider_ui.screen_emoij_label, &_sleepy_alpha_75x75);    
            break;
        case silly:
            lv_img_set_src(guider_ui.screen_emoij_label, &_silly_alpha_75x75);    
            break;
        case confused:
            lv_img_set_src(guider_ui.screen_emoij_label, &_confused_alpha_75x75);    
            break;                                                                                                                                                            

    }
    // display_font(" ");   
    // lv_obj_set_pos(guider_ui.screen_emoij_label, 70, 110);
}

void set_emoil_center()
{
    display_font(" ");  
    lv_obj_set_pos(guider_ui.screen_emoij_label, 80, 122);
}

void set_emoij_left()
{
    lv_obj_set_pos(guider_ui.screen_emoij_label, -1, 128);
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