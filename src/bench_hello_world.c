#include "bench_api.h"
#include "bench_utils.h"

bench_time_t timestamp_start_greeting;
bench_time_t timestamp_end_greeting;

void bench_hello(void)
{
	uint32_t diff;

	bench_timing_start();

	timestamp_start_greeting = bench_timing_counter_get();
	PRINT("Hello, world!\n");
	timestamp_end_greeting = bench_timing_counter_get();

	diff = bench_timing_cycles_get(&timestamp_start_greeting, &timestamp_end_greeting);
	PRINT_STATS("Time to greet", diff);

	PRINT_STATS_AVG("Average time to greet (with 1 sample)", diff, 1);

	bench_timing_stop();
}

/**
 * @brief Test setup function
 */
void bench_hello_init(void *arg)
{
	bench_timing_init();

	bench_hello();
}

int main(void)
{
	bench_test_init(bench_hello_init);
	return 0;
}