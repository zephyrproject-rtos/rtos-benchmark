add_subdirectory(arch)
add_subdirectory(timer)

target_sources(app PRIVATE bench_porting_layer_zephyr.c)
