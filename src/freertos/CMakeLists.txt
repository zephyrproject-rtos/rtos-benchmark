add_subdirectory(arch)
add_subdirectory(timer)

target_sources(app PRIVATE board.c)
target_sources(app PRIVATE clock_config.c)
target_sources(app PRIVATE pin_mux.c)
