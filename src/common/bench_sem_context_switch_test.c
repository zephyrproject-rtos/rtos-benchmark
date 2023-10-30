// SPDX-License-Identifier: Apache-2.0

/**
 * @file Measure time for semaphore give and take
 *
 * This file contains the test that measures semaphore give and
 * take time between threads.
 */

#include "bench_api.h"
#include "bench_utils.h"

#define MAIN_PRIORITY (BENCH_LAST_PRIORITY - 3)

static bench_time_t timestamp_start_sema_t_c;
static bench_time_t timestamp_end_sema_t_c;
static bench_time_t timestamp_start_sema_g_c;
static bench_time_t timestamp_end_sema_g_c;

static struct bench_stats take_times;
static struct bench_stats give_times;

/**
 * @brief Test main function.
 *
 * High priority thread that takes semaphore.
 */
void bench_sem_context_switch_high_prio_take(void *args)
{
	ARG_UNUSED(args);

	timestamp_start_sema_t_c = bench_timing_counter_get();
	bench_sem_take(0);
	timestamp_end_sema_g_c = bench_timing_counter_get();

	bench_thread_exit();
}

/**
 * @brief Test main function.
 *
 * Low priority thread that gives semaphore.
 */
void bench_sem_context_switch_low_prio_give(int priority, int iteration)
{
	bench_time_t diff;

	bench_thread_create(1, "high_prio_take", priority - 1,
			    bench_sem_context_switch_high_prio_take, NULL);
	bench_thread_start(1);

	timestamp_end_sema_t_c = bench_timing_counter_get();
	diff = bench_timing_cycles_get(&timestamp_start_sema_t_c,
				       &timestamp_end_sema_t_c);
	bench_stats_update(&take_times, diff, iteration);

	timestamp_start_sema_g_c = bench_timing_counter_get();
	bench_sem_give(0);
	diff = bench_timing_cycles_get(&timestamp_start_sema_g_c,
				       &timestamp_end_sema_g_c);
	bench_stats_update(&give_times, diff, iteration);
}

/**
 * @brief Test setup function
 */
void bench_sem_context_switch_init(void *arg)
{
	int i;

	bench_timing_init();
	bench_timing_start();

	bench_stats_reset(&take_times);
	bench_stats_reset(&give_times);
	bench_stats_report_title("Semaphore stats");

	bench_sem_create(0, 0, 1);

	bench_thread_set_priority(MAIN_PRIORITY);


	for (i = 1; i <= ITERATIONS; i++) {
		bench_sem_context_switch_low_prio_give(MAIN_PRIORITY, i);
		bench_collect_resources();
	}

	bench_stats_report_line("Take (context switch)", &take_times);
	bench_stats_report_line("Give (context switch)", &give_times);

	bench_timing_stop();
}

#ifdef RUN_SEM_CONTEXT_SWITCH
int main(void)
{
	PRINTF("\n\r *** Starting! ***\n\n\r");

	bench_test_init(bench_sem_context_switch_init);

	PRINTF("\n\r *** Done! ***\n\r");

	return 0;
}
#endif
