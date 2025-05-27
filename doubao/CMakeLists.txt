cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)

sdk_add_compile_definitions(-DSHELL_CMD_SIZE=512)
sdk_add_compile_definitions(-DASR_WAKEUP_INTERRUPT)
sdk_add_compile_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)

target_sources(app PRIVATE app_main.c)

add_subdirectory(lib)

add_subdirectory(lvgl)

sdk_add_include_directories(.)
sdk_add_include_directories(app)

sdk_set_main_file(lib/axk_app/main.c)

project(aipi-plachatv2_doubao)
