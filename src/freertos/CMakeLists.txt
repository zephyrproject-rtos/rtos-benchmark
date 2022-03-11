add_subdirectory(arch)

target_sources(app PRIVATE board.c)
target_sources(app PRIVATE clock_config.c)
target_sources(app PRIVATE pin_mux.c)
