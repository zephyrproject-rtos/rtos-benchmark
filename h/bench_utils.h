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
#include "../src/zephyr/bench_porting_layer_zephyr.h"
#endif /* ZEPHYR */
#ifdef FREERTOS
#include "../freertos/bench_porting_layer_freertos.h"
#endif /* FREERTOS */
#ifdef RTEMS
#include "../src/rtems/bench_porting_layer_rtems.h"
#endif /* RTEMS */
#ifdef __NuttX__
#include "../src/nuttx/bench_porting_layer_nuttx.h"
#endif /* __NuttX__ */
#ifdef __VXWORKS__
#include "../src/vxworks/bench_porting_layer_vxworks.h"
#endif

struct bench_stats {
	bench_time_t avg;
	bench_time_t min;
	bench_time_t max;
	bench_time_t total;
};

void bench_stats_reset(struct bench_stats *stats);

void bench_stats_update(struct bench_stats *stats, bench_time_t value,
			uint32_t iteration);

/**
 * @brief Display the test's title line
 */
void bench_stats_report_title(const char *title);

/**
 * @brief Display one line of statistics for a given test
 */
void bench_stats_report_line(const char *summary, const struct bench_stats *stats);

/**
 * @brief Display a line of "n/a" for a given test
 */
void bench_stats_report_na(const char *summary);

#endif
