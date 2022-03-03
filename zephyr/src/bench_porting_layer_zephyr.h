/* SPDX-License-Identifier: Apache-2.0 */

#ifndef PORTING_LAYER_ZEPHYR_H_
#define PORTING_LAYER_ZEPHYR_H_

#include <zephyr.h>
#include <kernel.h>
#include <timing/timing.h>
#include <stdio.h>

typedef timing_t bench_time_t;
typedef struct k_work bench_work;

#define TICK_SYNCH()  k_sleep(K_TICKS(1))

#define PRINTF(FMT, ...) printk(FMT, ##__VA_ARGS__)

#define BENCH_LAST_PRIORITY (CONFIG_NUM_PREEMPT_PRIORITIES - 1)

#endif
