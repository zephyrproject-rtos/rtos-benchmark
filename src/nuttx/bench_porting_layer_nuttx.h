/*
 * Copyright (c) 2022 Xiaomi Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PORTING_LAYER_NUTTX_H_
#define PORTING_LAYER_NUTTX_H_

#include <nuttx/config.h>
#include <nuttx/compiler.h>
#include <stdint.h>
#include <stdio.h>

#define BENCH_LAST_PRIORITY CONFIG_RTOS_BENCHMARK_PRIORITY
#define ITERATIONS CONFIG_RTOS_BENCHMARK_ITERATIONS
#define ARG_UNUSED(x) UNUSED(x)
#define __weak weak_function
#define PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)

typedef uint64_t bench_time_t;
typedef void * bench_work;

/*
 * Not all RTOSes support the same features. To help simplify the common code
 * as much as possible, where there are known differences in support, various
 * aspects of the benchmarking can be enabled/disabled as needed.
 */

#define RTOS_HAS_THREAD_SPAWN         1
#define RTOS_HAS_THREAD_CREATE_START  0
#define RTOS_HAS_SUSPEND_RESUME       0
#define RTOS_HAS_MAIN_ENTRY_POINT     1

#endif /* PORTING_LAYER_NUTTX_H_ */