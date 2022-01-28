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

uint32_t take_times[ITERATIONS];
uint32_t give_times[ITERATIONS];

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
	take_times[iteration] = diff;

	timestamp_start_sema_g_c = bench_timing_counter_get();
	bench_sem_give(0);
	diff = bench_timing_cycles_get(&timestamp_start_sema_g_c, &timestamp_end_sema_g_c);
	give_times[iteration] = diff;

	bench_timing_stop();
}

/**
 * @brief Test setup function
 */
void bench_sem_context_switch_init(void *arg)
{
	uint64_t avg, min, max;
	int i;

	bench_timing_init();

	bench_sem_create(0, 0, 1);

	bench_thread_set_priority(10); /* Lower main test thread priority */

	for (i = 0; i < ITERATIONS; i++) {
		bench_sem_context_switch_low_prio_give(i);
		thinker();
	}

	bench_stats(take_times, ITERATIONS, &avg, &min, &max);
	BENCH_PRINTF("Semaphore take cycles (context switch) [average, min, max],"
		     " %llu, %llu, %llu, Time, %llu %llu %llu\n", avg, min, max,
		     bench_timing_cycles_to_ns(avg),
		     bench_timing_cycles_to_ns(min),
		     bench_timing_cycles_to_ns(max));
	bench_stats(give_times, ITERATIONS, &avg, &min, &max);
	BENCH_PRINTF("Semaphore take cycles (context switch) [average, min, max],"
		     " %llu, %llu, %llu, Time, %llu, %llu, %llu\n", avg, min, max,
		     bench_timing_cycles_to_ns(avg),
		     bench_timing_cycles_to_ns(min),
		     bench_timing_cycles_to_ns(max));
}

int main(void)
{
	bench_test_init(bench_sem_context_switch_init);
	return 0;
}
