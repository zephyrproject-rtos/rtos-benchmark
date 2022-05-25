/* SPDX-License-Identifier: Apache-2.0 */

#ifndef PORTING_LAYER_FREERTOS_H_
#define PORTING_LAYER_FREERTOS_H_

#include "FreeRTOS.h"

typedef uint64_t bench_time_t;
typedef void * bench_work;

#define TICK_SYNCH()  k_sleep(K_TICKS(1))

#include "fsl_debug_console.h"

#define BENCH_LAST_PRIORITY (configMAX_PRIORITIES - 1)
#define BENCH_IDLE_TIME     5

#define __weak __attribute__((__weak__))

#define ARG_UNUSED(x) (void)(x)

/*
 * Not all RTOSes support the same features. To help simplify the common code
 * as much as possible, where there are known differences in support, various
 * aspects of the benchmarking can be enabled/disabled as needed.
 */

#define RTOS_HAS_THREAD_SPAWN         1
#define RTOS_HAS_THREAD_CREATE_START  0
#define RTOS_HAS_SUSPEND_RESUME       1
#define RTOS_HAS_MAIN_ENTRY_POINT     1

#endif /* PORTING_LAYER_FREERTOS_H_ */
