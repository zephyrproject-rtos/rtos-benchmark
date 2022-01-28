cmake_minimum_required(VERSION 3.22)

set(AVAILABLE_TESTS
    cooperative_context_switch
    interrupt_latency
    mutex_lock_unlock
    sem_context_switch
    sem_signal_release
    thread_switch_yield
    thread)

set(AVAILABLE_RTOSES
    zephyr
    freertos)

option(THINKER "If unrelated code should be executed between two iterations" ON)
if (THINKER)
    set(THINKER_STEPS 100 CACHE STRING "Number of iterations of thinker algorithm")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTHINKER_STEPS=${THINKER_STEPS} -DTHINKER_ENABLED")
endif()

if (NOT RTOS IN_LIST AVAILABLE_RTOSES)
    message(FATAL_ERROR "Choose either [zephyr] or [freertos]")
endif()

if (${RTOS} STREQUAL zephyr)
    set(BOARD qemu_x86)
    set(CONF_FILE zephyr/prj.conf)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DZEPHYR")

    find_package(Zephyr 2.7.0 HINTS $ENV{ZEPHYR_BASE})
    target_sources(app PRIVATE zephyr/src/bench_porting_layer_zephyr.c)
else()
    if (NOT MCUX_SDK_PATH)
        message(FATAL_ERROR "Please, inform MCUXpresso SDK path via MCUX_SDK_PATH")
    endif()

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFREERTOS")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DCPU_MK64FN1M0VLL12")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSERIAL_PORT_TYPE_UART=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=cortex-m4")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=fpv4-sp-d16")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mthumb")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffreestanding")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__STARTUP_CLEAR_BSS")

    set(CMAKE_TOOLCHAIN_FILE ${MCUX_SDK_PATH}/tools/cmake_toolchain_files/armgcc.cmake)

    enable_language(ASM)

    include_directories(freertos)
    include_directories(${MCUX_SDK_PATH}/CMSIS/Core/Include)
    include_directories(${MCUX_SDK_PATH}/components/serial_manager)
    include_directories(${MCUX_SDK_PATH}/components/uart)
    include_directories(${MCUX_SDK_PATH}/devices/MK64F12)
    include_directories(${MCUX_SDK_PATH}/devices/MK64F12/drivers)
    include_directories(${MCUX_SDK_PATH}/devices/MK64F12/utilities/debug_console)
    include_directories(${MCUX_SDK_PATH}/devices/MK64F12/utilities/str)
    include_directories(${MCUX_SDK_PATH}/rtos/freertos/freertos_kernel/include)
    include_directories(${MCUX_SDK_PATH}/rtos/freertos/freertos_kernel/portable/GCC/ARM_CM4F)

    add_executable(app freertos/bench_porting_layer_freertos.c)

    target_sources(app PRIVATE freertos/board.c)
    target_sources(app PRIVATE freertos/clock_config.c)
    target_sources(app PRIVATE freertos/pin_mux.c)

    target_sources(app PRIVATE ${MCUX_SDK_PATH}/components/serial_manager/fsl_component_serial_manager.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/components/serial_manager/fsl_component_serial_port_uart.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/components/uart/fsl_adapter_uart.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/devices/MK64F12/drivers/fsl_clock.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/devices/MK64F12/drivers/fsl_smc.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/devices/MK64F12/drivers/fsl_uart.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/devices/MK64F12/gcc/startup_MK64F12.S)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/devices/MK64F12/system_MK64F12.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/devices/MK64F12/utilities/debug_console/fsl_debug_console.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/devices/MK64F12/utilities/debug_console/fsl_debug_console.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/devices/MK64F12/utilities/fsl_sbrk.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/devices/MK64F12/utilities/str/fsl_str.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/rtos/freertos/freertos_kernel/list.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/rtos/freertos/freertos_kernel/portable/GCC/ARM_CM4F/port.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/rtos/freertos/freertos_kernel/portable/MemMang/heap_4.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/rtos/freertos/freertos_kernel/queue.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/rtos/freertos/freertos_kernel/stream_buffer.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/rtos/freertos/freertos_kernel/tasks.c)
    target_sources(app PRIVATE ${MCUX_SDK_PATH}/rtos/freertos/freertos_kernel/timers.c)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --specs=nano.specs")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --specs=nosys.specs")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${CMAKE_CURRENT_SOURCE_DIR}/freertos/armgcc/MK64FN1M0xxx12_flash.ld")

    target_link_libraries(app PRIVATE -Wl,--start-group)
    target_link_libraries(app PRIVATE c)
    target_link_libraries(app PRIVATE gcc)
    target_link_libraries(app PRIVATE nosys)
    target_link_libraries(app PRIVATE -Wl,--end-group)
endif()

project(bench)

list(LENGTH TEST tests_to_run)
if (NOT ${tests_to_run} EQUAL 1)
    message(FATAL_ERROR "Choose exactly one test")
endif()

if (NOT ${TEST} IN_LIST AVAILABLE_TESTS)
    list(JOIN AVAILABLE_TESTS "\n" pretty_print)
    message(FATAL_ERROR "Choose one test from:\n${pretty_print}")
endif()

target_sources(app PRIVATE src/bench_utils.c)
target_sources(app PRIVATE src/bench_${TEST}_test.c)