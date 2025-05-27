sdk_generate_library()

sdk_add_include_directories(.)
sdk_add_include_directories(generated)
sdk_add_include_directories(generated/guider_customer_fonts)
sdk_add_include_directories(custom)

sdk_library_add_sources( 
    custom/custom.c 
    generated/guider_fonts/lv_font_SourceHanSerifSC_Regular_20.c
    generated/images/_no_internet_alpha_30x30.c
    generated/images/_robot_alpha_100x100.c
    generated/events_init.c
    generated/gui_guider.c
    generated/setup_scr_screen.c
    generated/widgets_init.c
    lcd_display.c
    generated/guider_customer_fonts/lv_customer_font_SourceHanSerifSC_Regular_20.c
    generated/images/_wifi_connected_alpha_35x28.c
    generated/images/_face_frown_0_alpha_100x100.c
    generated/images/_smile_0_alpha_100x100.c
    generated/images/_sleep_0_alpha_100x100.c
)