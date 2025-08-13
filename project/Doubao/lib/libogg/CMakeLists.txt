sdk_generate_library()

sdk_add_include_directories(.)

sdk_add_private_include_directories( ./ogg )

sdk_library_add_sources( framing.c )
