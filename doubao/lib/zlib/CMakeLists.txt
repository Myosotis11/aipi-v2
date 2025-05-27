sdk_generate_library()

sdk_add_include_directories(.)

# sdk_add_private_compile_options(-Wall -O2  -Wno-unused-function)

# sdk_add_private_compile_definitions(
#     -DHAVE_CONFIG_H
#     -DFIXED_POINT
#     # -DOPUS_CSKY_ASM
#     -DDISABLE_FLOAT_API
#     -DHAVE_ALLOCA_H
#     -DHAVE_LRINT
#     -DHAVE_LRINTF 
#     -DHAVE_MEMORY_H
#     -DUSE_ALLOCA
#     -DOPUS_BUILD
#     )

sdk_library_add_sources(
    adler32.c
    deflate.c
    inflate.c
    inftrees.c
    inffast.c
    trees.c
    zutil.c
    )
