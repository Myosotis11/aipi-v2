sdk_add_include_directories(.)

target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_flash_psram.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_gpio.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/fw_header.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/axk_board.c)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/axk_main.o)

if(CONFIG_RF)
    target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/board_rf.c)
endif()

sdk_set_linker_script(bl616_psram_doubao.ld)

if(CONFIG_PSRAM)
    sdk_add_compile_definitions(-D CONFIG_PSRAM)
endif()

if(CONFIG_CONSOLE_WO)
    sdk_add_compile_definitions(-D CONFIG_CONSOLE_WO)
endif()

if(CONFIG_BOARD_FLASH_LOW_SPEED)
    sdk_add_compile_definitions(-DCONFIG_BOARD_FLASH_LOW_SPEED)
endif()

sdk_add_link_options(-ufw_header)

# fw tools board directory select
if(CONFIG_CUSTOMIZE_BOARD_DIR)
    set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_DIR ${CONFIG_CUSTOMIZE_BOARD_DIR})
else()
    set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# fw tools board config select
if(CONFIG_CUSTOMIZE_BOARD_CONFIG)
    set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_CONFIG ${CONFIG_CUSTOMIZE_BOARD_CONFIG})
else()
    set_property(GLOBAL PROPERTY FW_TOOL_CUSTOM_BOARD_CONFIG config)
endif()
