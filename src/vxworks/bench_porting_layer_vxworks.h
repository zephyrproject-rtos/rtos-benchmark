/*
 * Copyright (c) 2023 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PORTING_LAYER_VXWORKS_H_
#define PORTING_LAYER_VXWORKS_H_

#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <syscall.h>
#include <errno.h>
#include <cpusetCommon.h>
#include <tickLib.h>
#include <sysLib.h>
#include <taskLib.h>
#include <semLib.h>
#include <msgQLib.h>
#include <private/schedP.h>
#include <private/clockLibP.h>

#define CONFIG_RTOS_BENCHMARK_PRIORITY      rtpMainPri
#define CONFIG_RTOS_BENCHMARK_ITERATIONS    1000
#define CONFIG_RTOS_BENCHMARK_MAXTHREADS    20
#define CONFIG_RTOS_BENCHMARK_MAXSEMAPHORES 20
#define CONFIG_RTOS_BENCHMARK_MAXMUTEXES    10
#define CONFIG_RTOS_BENCHMARK_MAXMSGQS      20

#define BENCH_LAST_PRIORITY CONFIG_RTOS_BENCHMARK_PRIORITY
#define ITERATIONS          CONFIG_RTOS_BENCHMARK_ITERATIONS
#define ARG_UNUSED(x)       (void)(x)
#define __weak
#define PRINTF(fmt, ...)    printf(fmt, ##__VA_ARGS__)

/*
 * Not all RTOSes support the same features. To help simplify the common code
 * as much as possible, where there are known differences in support, various
 * aspects of the benchmarking can be enabled/disabled as needed.
 */

#define RTOS_HAS_THREAD_SPAWN         1
#define RTOS_HAS_MESSAGE_QUEUE        1

#ifdef VXWORKS_POSIX
#define RTOS_HAS_THREAD_CREATE_START  0
#define RTOS_HAS_SUSPEND_RESUME       0
#else
#define RTOS_HAS_THREAD_CREATE_START  1
#define RTOS_HAS_SUSPEND_RESUME       1
#endif /* VXWORKS_POSIX */

#define RTOS_HAS_MAIN_ENTRY_POINT     0

#define NSEC_PER_SEC       1000000000
#define USEC_PER_SEC       1000000

#define TASK_STACK_SIZE  (size_t) 10000

typedef uint64_t bench_time_t;
typedef void * bench_work;

typedef enum       /* TIMER_TYPE - timer used for timestamping */
    {
    TIMER_ARM_PMU     = 0,   /* ARM PMU */
    TIMER_POSIX_CLOCK = 1    /* high-res POSIX clock */
    } TIMER_TYPE;

extern _Vx_freq_t    tickClkRate;
extern uint64_t      timerFreq;
extern int           rtpMainPri;
extern TIMER_TYPE    timerType;

bench_time_t time_cnt_get(void);

#endif /* PORTING_LAYER_VXWORKS_H_ */
