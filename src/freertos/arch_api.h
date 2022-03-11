#include "bench_api.h"

void arch_timing_init(void);
void arch_timing_start(void);
void arch_timing_stop(void);
bench_time_t arch_timing_counter_get(void);
bench_time_t arch_timing_cycles_get(volatile bench_time_t *const start,
				    volatile bench_time_t *const end);
bench_time_t arch_timing_cycles_to_ns(bench_time_t cycles);
