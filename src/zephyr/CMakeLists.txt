add_subdirectory(arch)

target_sources(app PRIVATE bench_porting_layer_zephyr.c)
