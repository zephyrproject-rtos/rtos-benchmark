/* SPDX-License-Identifier: Apache-2.0 */

#ifndef PORTING_LAYER_ZEPHYR_H_
#define PORTING_LAYER_ZEPHYR_H_

#include <timing/timing.h>
#include <zephyr.h>
#include <kernel.h>
#include <stdio.h>

typedef timing_t bench_time_t;
typedef struct k_work bench_work;

#define TICK_SYNCH()  k_sleep(K_TICKS(1))

#define PRINT(...)						\
	{							\
		printk("%s", ##__VA_ARGS__);					\
	}

#define PRINT_F(...)						\
	{							\
		char sline[256]; 				\
		snprintk(sline, 256, FORMAT, ##__VA_ARGS__); 	\
		printk("%s", sline);					\
	}

#endif
