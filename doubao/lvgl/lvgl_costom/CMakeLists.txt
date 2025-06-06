sdk_generate_library()

sdk_add_include_directories(.)
sdk_add_include_directories(generated)
sdk_add_include_directories(generated/guider_customer_fonts)
sdk_add_include_directories(custom)

sdk_library_add_sources(
    custom/custom.c
    generated/guider_fonts/lv_font_montserratMedium_16.c
    generated/guider_fonts/lv_font_montserratMedium_20.c
    generated/events_init.c
    generated/gui_guider.c
    generated/setup_scr_screen.c
    generated/widgets_init.c
    lcd_display.c
)