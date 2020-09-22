cmake_minimum_required(VERSION 3.8.2)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)


target_sources(app PRIVATE bench_sem_signal_release_test.c)
target_sources(app PRIVATE bench_porting_layer_zephyr.c)
target_compile_definitions(app PRIVATE ENABLE_PRINTF=1)
