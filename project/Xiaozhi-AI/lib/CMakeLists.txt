# sdk_generate_library()

sdk_add_include_directories(.)
sdk_add_static_library(libdev_bl616.a libaxk_ota.a libblufi.a liblibogg.a libopus.a libVB6824.a libxiaozhi.a libboard.a)

target_sources(app PRIVATE axk_main.c.o)