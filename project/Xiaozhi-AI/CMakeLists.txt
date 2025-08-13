cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

# User
sdk_add_compile_definitions(-DCONFIG_CLI_CMD_ENABLE)
# sdk_add_compile_options(-DMQTT_USE_CUSTOM_SOCKET_HANDLE)
# sdk_add_compile_definitions(-DMBEDTLS_PK_PARSE_C)
# sdk_add_compile_definitions(-DBL616_DHCP_DEBUG)

sdk_add_compile_definitions(-DSHELL_CMD_SIZE=512)
sdk_add_compile_definitions(-DASR_WAKEUP_INTERRUPT)
sdk_add_compile_definitions(-DENABLE_SSL)
sdk_add_compile_definitions(-DLV_LVGL_H_INCLUDE_SIMPLE)

add_subdirectory(lvgl)
add_subdirectory(Storage)

sdk_add_static_library(./lib/libdev_bl616.a ./lib/libaxk_ota.a ./lib/libblufi.a ./lib/libdev_bl616.a ./lib/liblibogg.a ./lib/libopus.a ./lib/libVB6824.a ./lib/libxiaozhi.a)

target_sources(app PRIVATE app_main.c  xiaozhi_api.c )

sdk_add_include_directories(.)
sdk_add_include_directories(include)
sdk_add_include_directories(include/opus)
sdk_add_include_directories(include/ogg)

sdk_set_main_file(main.c)

project(aipi-plachatv2_xiaozhi)
