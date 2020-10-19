/* SPDX-License-Identifier: Apache-2.0 */

#ifndef PORTING_LAYER_ZEPHYR_H_
#define PORTING_LAYER_ZEPHYR_H_

#include <timing/timing.h>
#include <zephyr.h>
#include <kernel.h>

typedef timing_t bench_time_t;
typedef struct k_work bench_work;

#define TICK_SYNCH()  k_sleep(K_TICKS(1))

#endif
