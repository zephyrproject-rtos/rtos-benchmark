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
#include "arch_api.h"

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
	arch_timing_init();
}

void bench_timing_start(void)
{
	arch_timing_start();
}

void bench_timing_stop(void)
{
	arch_timing_stop();
}

bench_time_t bench_timing_counter_get(void)
{
	return arch_timing_counter_get();
}

bench_time_t bench_timing_cycles_get(bench_time_t *time_start, bench_time_t *time_end)
{
	return arch_timing_cycles_get(time_start, time_end);
}

bench_time_t bench_timing_cycles_to_ns(bench_time_t cycles)
{
	return arch_timing_cycles_to_ns(cycles);
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

void bench_sem_give_from_isr(int sem_id)
{
	xSemaphoreGiveFromISR(semaphores[sem_id], NULL);
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

void bench_thread_exit(void)
{
	vTaskDelete(NULL);
}

void bench_sleep(uint32_t msec)
{
	TickType_t delay = msec / portTICK_PERIOD_MS;
	vTaskDelay(delay);
}
