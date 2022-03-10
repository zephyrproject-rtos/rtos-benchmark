#include "bench_api.h"
#include "bench_porting_layer_freertos.h"

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "board.h"

#include "pin_mux.h"
#include "clock_config.h"

#include <assert.h>

#define MAX_SEMAPHORES 2
#define MAX_THREADS 10
#define STACK_SIZE 128
#define MAX_MUTEXES 1


static SemaphoreHandle_t semaphores[MAX_SEMAPHORES];
static TaskHandle_t threads[MAX_THREADS];
static SemaphoreHandle_t mutexes[MAX_MUTEXES];

#define benchmark_task_PRIORITY (configMAX_PRIORITIES - 1)

void bench_test_init(void (*test_init_function)(void *))
{
	/* Init board hardware. */
	BOARD_InitBootPins();
	BOARD_InitBootClocks();
	BOARD_InitDebugConsole();
	if (xTaskCreate(test_init_function, "benchmark", configMINIMAL_STACK_SIZE + 100, NULL, benchmark_task_PRIORITY, NULL) !=
		pdPASS)
	{
		PRINTF("Task creation failed!.\r\n");
		while (1)
			;
	}
	vTaskStartScheduler();
	for (;;)
		;
}

void bench_timing_init(void)
{
	KIN1_InitCycleCounter();
}

void bench_timing_start(void)
{
	KIN1_ResetCycleCounter();
}

void bench_timing_stop(void)
{
	KIN1_DisableCycleCounter();
}

bench_time_t bench_timing_counter_get(void)
{
	bench_time_t cycles = KIN1_GetCycleCounter(); // Copy from volatile
	return cycles;
}

bench_time_t bench_timing_cycles_get(bench_time_t *time_start, bench_time_t *time_end)
{
	return (*time_end - *time_start);
}

bench_time_t bench_timing_cycles_to_ns(bench_time_t cycles)
{
	// unsigned int timing_freq = 
	// unsigned int timing_freq_mhz = (unsigned int)(timing_freq / 1000000);

	// return (uint32_t)(arch_timing_freq_get() / 1000000);
	// return (cycles) * (NSEC_PER_USEC) / arch_timing_freq_get_mhz();
	return 0;
}

int bench_sem_create(int sem_id, int initial_count, int maximum_count)
{
	semaphores[sem_id] = xSemaphoreCreateCounting(maximum_count, initial_count);
	return BENCH_SUCCESS;
}

void bench_sem_give(int sem_id)
{
	xSemaphoreGive(semaphores[sem_id]);
	return;
}

int bench_sem_take(int sem_id)
{
	xSemaphoreTake(semaphores[sem_id], portMAX_DELAY);
	return BENCH_SUCCESS;
}

void *bench_malloc(size_t size)
{
	return pvPortMalloc(size);
}

void bench_free(void *ptr)
{
	vPortFree(ptr);
}

void bench_thread_start(int thread_id)
{
	ARG_UNUSED(thread_id);

	// There's no thread start concept on FreeRTOS - a task is started
	// when it's created. So this is a no-op.
}

static UBaseType_t map_prio(int prio)
{
	// FreeRTOS priorities have smaller numbers with lower priorities
	// So we need to remap a small number to a big one, and vice versa

	assert(prio >= 0);

	return configMAX_PRIORITIES - prio;
}

void bench_thread_set_priority(int priority)
{
	vTaskPrioritySet(NULL, map_prio(priority));
}

int bench_thread_create(int thread_id, const char *thread_name, int priority,
	void (*entry_function)(void *), void *args)
{
	BaseType_t ret;

	if (thread_id < 0 || thread_id > MAX_THREADS)
		return BENCH_ERROR;

	ret = xTaskCreate(entry_function, thread_name, STACK_SIZE, args,
			  map_prio(priority), &threads[thread_id]);

	if (ret != pdPASS)
		return BENCH_ERROR;

	return BENCH_SUCCESS;
}

void bench_thread_resume(int thread_id)
{
	vTaskResume(threads[thread_id]);
}

void bench_thread_suspend(int thread_id)
{
	vTaskSuspend(threads[thread_id]);
}

void bench_thread_abort(int thread_id)
{
	vTaskDelete(threads[thread_id]);
}

void bench_yield(void)
{
	taskYIELD();
}

int bench_mutex_create(int mutex_id)
{
	assert(mutex_id < MAX_MUTEXES);

	mutexes[mutex_id] = xSemaphoreCreateRecursiveMutex();
}

int bench_mutex_lock(int mutex_id)
{
	xSemaphoreTakeRecursive(mutexes[mutex_id], portMAX_DELAY);
}

int bench_mutex_unlock(int mutex_id)
{
	xSemaphoreGiveRecursive(mutexes[mutex_id]);
}

void bench_sync_ticks(void)
{
}

bench_isr_handler_t bench_timer_isr_get(void)
{
	return NULL;
}

void bench_timer_isr_set(bench_isr_handler_t handler)
{
	ARG_UNUSED(handler);
}

void bench_timer_isr_restore(bench_isr_handler_t handler)
{
	ARG_UNUSED(handler);
}

uint64_t bench_timer_isr_expiry_set(uint32_t usec)
{
	ARG_UNUSED(usec);

	return 0;
}

uint64_t bench_timer_cycles_diff(uint64_t trigger, uint64_t sample)
{
	ARG_UNUSED(trigger);
	ARG_UNUSED(sample);

	return 0;
}

bench_time_t bench_timer_cycles_get(void)
{
	return 0;
}

uint32_t bench_timer_cycles_per_second(void)
{
	return 0;
}

uint32_t bench_timer_cycles_per_tick(void)
{
	return 0;
}

void bench_thread_exit(void)
{
	vTaskDelete(NULL);
}

void bench_sleep(uint32_t msec)
{
	TickType_t delay = msec / portTICK_PERIOD_MS;
	vTaskDelay(delay);
}
