/* SPDX-License-Identifier: Apache-2.0 */

#ifndef _LATENCY_MEASURE_UNIT_H
#define _LATENCY_MEASURE_UNIT_H

#include <stdio.h>

#ifdef CSV_FORMAT_OUTPUT
#define FORMAT "%-50s,%8u,%8u\n"
#else
#define FORMAT "%-50s:%8u cycles , %8u ns\n"
#endif

#ifdef ZEPHYR
#include "../zephyr/src/bench_porting_layer_zephyr.h"
#endif /* ZEPHYR */
#ifdef FREERTOS
#include "../freertos/bench_porting_layer_freertos.h"
#endif /* FREERTOS */

struct bench_stats {
	bench_time_t avg;
	bench_time_t min;
	bench_time_t max;
	bench_time_t total;
	bench_time_t calibration;
};

void bench_stats_reset(struct bench_stats *stats);

void bench_stats_update(struct bench_stats *stats, bench_time_t value,
			uint32_t iteration);

#ifdef THINKER_ENABLED
void thinker(void);
#else
#define thinker() do { } while (false)
#endif

#endif
