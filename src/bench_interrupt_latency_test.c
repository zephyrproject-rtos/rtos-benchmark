// SPDX-License-Identifier: Apache-2.0

#include "bench_api.h"
#include "bench_utils.h"

static volatile int flag_var;

static bench_time_t timestamp_start;
static bench_time_t timestamp_end;

static void bench_latency_test_isr(const void *unused)
{
	ARG_UNUSED(unused);
	flag_var = 1;

	timestamp_start = bench_timing_counter_get();
}

static void bench_make_interrupt(void)
{
	flag_var = 0;
	irq_offload(bench_latency_test_isr, NULL);
	if (flag_var != 1) {
		printf("Flag variable has not changed. Interrupt latency test FAILED\n");
	} else {
		timestamp_end = bench_timing_counter_get();
	}
}

void bench_interrupt_to_thread(void)
{
	uint32_t diff;

	bench_timing_start();
	bench_sync_ticks();

	bench_make_interrupt();
	if (flag_var == 1) {
		diff = bench_timing_cycles_get(timestamp_start, timestamp_end);
		PRINT_STATS("Switch from ISR back to interrupted thread", diff);
	}

	bench_timing_stop();
}

void bench_interrupt_latency_init(void)
{
	bench_timing_init();

	bench_interrupt_to_thread();
}

int main(void)
{
	bench_test_init(bench_interrupt_latency_init);
	return 0;
}
