## OLD
# cmake_minimum_required(VERSION 3.8.2)

# include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)


cmake_minimum_required(VERSION 3.13.1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(NONE)

set(SRC_DIR ../src/)
set(ZEPHYR_LAYER_SRC_DIR src/)

SET(SdkDirPath ${CMAKE_CURRENT_SOURCE_DIR}/../freertos/SDK_2.8.2_FRDM-K64F)

# target_sources(app PRIVATE ${SRC_DIR}/bench_cooperative_context_switch_test.c)
# target_sources(app PRIVATE ${SRC_DIR}/bench_interrupt_latency_test.c)
# target_sources(app PRIVATE ${SRC_DIR}/bench_interrupt_offload_latency_test.c)
# target_sources(app PRIVATE ${SRC_DIR}/bench_mutex_lock_unlock_test.c)
# target_sources(app PRIVATE ${SRC_DIR}/bench_sem_context_switch_test.c)
# target_sources(app PRIVATE ${SRC_DIR}/bench_sem_signal_release_test.c)
# target_sources(app PRIVATE ${SRC_DIR}/bench_thread_switch_yield_test.c)
# target_sources(app PRIVATE ${SRC_DIR}/bench_thread_test.c)
target_sources(app PRIVATE ${SRC_DIR}/bench_hello_world.c)

target_sources(app PRIVATE ${ZEPHYR_LAYER_SRC_DIR}/bench_porting_layer_zephyr.c)

target_compile_definitions(app PRIVATE ENABLE_PRINTF=1 ZEPHYR)
