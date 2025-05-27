# sdk_generate_library()

sdk_add_include_directories(.)

target_sources(app PRIVATE volc_user.c axk_ringbuff.c stoge.c)
target_sources(app PRIVATE axk_server.o VolcEngineRTCLiteDemo.o media_6824.o vb6824.o)
