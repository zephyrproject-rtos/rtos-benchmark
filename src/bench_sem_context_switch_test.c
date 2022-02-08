// SPDX-License-Identifier: Apache-2.0

/**
 * @file Measure time for semaphore give and take
 *
 * This file contains the test that measures semaphore give and
 * take time between threads.
 */

#include "bench_api.h"
#include "bench_utils.h"

bench_time_t timestamp_start_sema_t_c;
bench_time_t timestamp_end_sema_t_c;
bench_time_t timestamp_start_sema_g_c;
bench_time_t timestamp_end_sema_g_c;

struct bench_stats take_times;
struct bench_stats give_times;

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
}

/**
 * @brief Test main function.
 *
 * Low priority thread that gives semaphore.
 */
void bench_sem_context_switch_low_prio_give(int iteration)
{
	uint32_t diff;

	bench_timing_start();

	bench_thread_create(1, "high_prio_take", 3, bench_sem_context_switch_high_prio_take, NULL);
	bench_thread_start(1);

	timestamp_end_sema_t_c = bench_timing_counter_get();
	diff = bench_timing_cycles_get(&timestamp_start_sema_t_c, &timestamp_end_sema_t_c);
	bench_stats_update(&take_times, diff, iteration);

	timestamp_start_sema_g_c = bench_timing_counter_get();
	bench_sem_give(0);
	diff = bench_timing_cycles_get(&timestamp_start_sema_g_c, &timestamp_end_sema_g_c);
	bench_stats_update(&give_times, diff, iteration);

	bench_timing_stop();
}

/**
 * @brief Test setup function
 */
void bench_sem_context_switch_init(void *arg)
{
	int i;

	bench_timing_init();

	bench_stats_reset(&take_times);
	bench_stats_reset(&give_times);

	bench_sem_create(0, 0, 1);

	bench_thread_set_priority(10); /* Lower main test thread priority */

	for (i = 1; i <= ITERATIONS; i++) {
		bench_sem_context_switch_low_prio_give(i);
		thinker();
	}

	BENCH_PRINTF("Semaphore take time (context switch) [avg min max], "
		     " %llu, %llu, %llu\n",
		     bench_timing_cycles_to_ns(take_times.avg),
		     bench_timing_cycles_to_ns(take_times.min),
		     bench_timing_cycles_to_ns(take_times.max));
	BENCH_PRINTF("Semaphore give cycles (context switch) [avg min max], "
		     " %llu, %llu, %llu\n",
		     bench_timing_cycles_to_ns(give_times.avg),
		     bench_timing_cycles_to_ns(give_times.min),
		     bench_timing_cycles_to_ns(give_times.max));
}

int main(void)
{
	bench_test_init(bench_sem_context_switch_init);
	return 0;
}
