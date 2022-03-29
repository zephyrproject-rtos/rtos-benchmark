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
 * Some RTOSes distinguish between creating, starting and spawning a thread.
 * FreeRTOS does not. Thus creating a thread within FreeRTOS is equivalent
 * to spawning a thread. These macros come into play when reporting results.
 */

#define RTOS_HAS_THREAD_SPAWN         1
#define RTOS_HAS_THREAD_CREATE_START  0

#endif /* PORTING_LAYER_FREERTOS_H_ */
