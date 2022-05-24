/* SPDX-License-Identifier: Apache-2.0 */

#ifndef PORTING_LAYER_RTEMS_H_
#define PORTING_LAYER_RTEMS_H_

/* includes */

#include <stdint.h>
#include <stddef.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdbool.h>

/* typedefs */

typedef uint64_t bench_time_t;

/* defines */

#define PRINTF(FMT, ...)    printf(FMT, ##__VA_ARGS__)
#define ARG_UNUSED(x)       (void)(x)
#define BENCH_LAST_PRIORITY 10

#define __weak __attribute__((__weak__))

/*
 * Not all RTOSes support the same features. To help simplify the common code
 * as much as possible, where there are known differences in support, various
 * aspects of the benchmarking can be enabled/disabled as needed.
 */

#define RTOS_HAS_THREAD_SPAWN         0
#define RTOS_HAS_THREAD_CREATE_START  1
#define RTOS_HAS_SUSPEND_RESUME       1
#define RTOS_HAS_MAIN_ENTRY_POINT     0

#endif
