/* SPDX-License-Identifier: Apache-2.0 */

#ifndef  BENCH_API_H
#define  BENCH_API_H

#define BENCH_SUCCESS 0 /* Value returned when operation succeeds */
#define BENCH_ERROR 1 /* Value returned when operation fails */

#ifdef ZEPHYR
#include "../src/zephyr/bench_porting_layer_zephyr.h"
#endif /* ZEPHYR */
#ifdef FREERTOS
#include "../freertos/bench_porting_layer_freertos.h"
#endif /* FREERTOS */

typedef void (*bench_isr_handler_t)(void *arg);

/**
 * @brief Call test initialization function
 *
 * This routine calls the initialization function of the test.
 *
 * @param test_init_function     The test initialization function
 */
void bench_test_init(void (*test_init_function)(void *));

/**
 * @brief Set priority of current thread
 *
 * This routine immediately changes the priority of the current thread.
 *
 * @param priority New priority
 */
void bench_thread_set_priority(int priority);

/**
 * @brief Create a thread and set its name
 *
 * This routine initializes a thread and sets its name. It does not schedule the thread.
 *
 * @param thread_id       Handle for thread.
 * @param thread_name     Name of thread.
 * @param priority        Thread priority.
 * @param entry_function  Thread entry function.
 * @param args            Entry point parameter representing arguments.
 *
 * @return BENCH_SUCCESS on success or BENCH_FAILURE on failure
 */
int bench_thread_create(int thread_id, const char *thread_name, int priority,
	void (*entry_function)(void *), void *args);

/**
 * @brief Start an initialized but unstarted thread
 *
 * This routine starts the thread by adding it to the scheduler queue for the first time.
 *
 * @param thread_id Handle of thread
 */
void bench_thread_start(int thread_id);

/**
 * @brief Resume a suspended thread
 *
 * This routine resumes a thread that has been suspended.
 *
 * @param thread_id Handle of thread
 */
void bench_thread_resume(int thread_id);

/**
 * @brief Suspend a thread.
 *
 * This routine suspends the thread.
 *
 * @param thread_id Handle of thread
 */
void bench_thread_suspend(int thread_id);

/**
 * @brief Abort a thread
 *
 * This routine aborts the thread.
 *
 * @param thread_id Handle of thread
 */
void bench_thread_abort(int thread_id);

/**
 * @brief Exits from a thread
 *
 * This routine exits current thread, and clean up any resources.
 */
void bench_thread_exit(void);

/**
 * @brief Yield the current thread
 *
 * This routine causes the current thread to yield execution to another thread of
 * the same or higher priority.
 */
void bench_yield(void);

/**
 * @brief Set up to offload work from ISR
 *
 * This routine performs setup to enable \ref bench_offload_submit_work to
 * offload work from ISR to another thread, such as setting up a workqueue to place work items into.
 */
void bench_offload_setup(void);

/**
 * @brief Initialize the work item representing work to be offloaded from ISR
 *
 * This routine initializes and stores the work item that represents the work to be offloaded.
 *
 * @param worker_function Function to call
 */
void bench_offload_create_work(void (*worker_function)(bench_work *));

/**
 * @brief Offload work from ISR
 *
 * This routine executes the stored work item on another thread from ISR.
 */
void bench_offload_submit_work(void);


/**
 * @brief Initialize timing
 *
 * This routine performs setup to enable \ref bench_timing_start.
 */
void bench_timing_init(void);

/**
 * @brief Synchronize ticks
 *
 * This routine synchronizes ticks by sleeping a tick.
 */
void bench_sync_ticks(void);

/**
 * @brief Start timing
 *
 * This routine performs setup to enable routines that get time measurements.
 */
void bench_timing_start(void);

/**
 * @brief Stop timing
 *
 * This routine performs setup to disable routines that get time measurements.
 */
void bench_timing_stop(void);

/**
 * @brief Read the hardware clock
 *
 * This routine returns the current time, as measured by the system's hardware clock.
 *
 * @return Current hardware clock up counter (in cycles)
 */
bench_time_t bench_timing_counter_get(void);

/**
 * @brief Get number of cycles between @p time_start and @p time_end
 *
 * @param time_start Pointer to counter at start of a measured execution
 * @param time_end Pointer to counter at stop of a measured execution
 * @return Number of cycles between start and end
 */
bench_time_t bench_timing_cycles_get(bench_time_t *time_start, bench_time_t *time_end);

/**
 * @brief Convert number of @p cycles into nanoseconds
 *
 * @param cycles Number of cycles
 * @return Converted time value
 */
bench_time_t bench_timing_cycles_to_ns(bench_time_t cycles);

/**
 * @brief Create a semaphore
 *
 * This routine initializes a semaphore object, prior to its first use.
 *
 * @param sem_id          ID of semaphore (to be used with other routines)
 * @param initial_count   Initial semaphore count
 * @param maximum_count   Maximum permitted semaphore count
 * @return BENCH_SUCCESS on success or BENCH_FAILURE on failure
 */
int bench_sem_create(int sem_id, int initial_count, int maximum_count);

/**
 * @brief Give a semaphore
 *
 * This routine gives the semaphore, unless the semaphore is already at its maximum permitted count.
 *
 * @param sem_id ID of semaphore
 */
void bench_sem_give(int sem_id);

/**
 * @brief Give a semaphore from inside an ISR
 *
 * This routine gives the semaphore, unless the semaphore is already at its maximum permitted count.
 *
 * @param sem_id ID of semaphore
 */
void bench_sem_give_from_isr(int sem_id);

/**
 * @brief Take a semaphore
 *
 * This routine takes the semaphore without any timeout period. If the semaphore is
 * never incremented, the routine will wait forever.
 *
 * @param sem_id ID of semaphore
 * @return BENCH_SUCCESS on success or BENCH_FAILURE on failure
 */
int bench_sem_take(int sem_id);

/**
 * @brief Create a mutex
 *
 * This routine creates a mutex object, prior to its first use.
 *
 * @param mutex_id ID of mutex (to be used with other routines)
 * @return BENCH_SUCCESS on success or BENCH_FAILURE on failure
 */
int bench_mutex_create(int mutex_id);

/**
 * @brief Lock a mutex
 *
 * This routine locks a mutex.
 *
 * @param mutex_id ID of mutex
 * @return BENCH_SUCCESS on success or BENCH_FAILURE on failure
 */
int bench_mutex_lock(int mutex_id);

/**
 * @brief Unlock a mutex
 *
 * This routine unlocks a mutex.
 *
 * @param mutex_id ID of mutex
 * @return BENCH_SUCCESS on success or BENCH_FAILURE on failure
 */
int bench_mutex_unlock(int mutex_id);

/**
 * @brief Allocate memory from the heap.
 *
 * @param size Size of memory to be allocated.
 * @return memory allocated or NULL in case of error.
 */
void *bench_malloc(size_t size);

/**
 * @brief Free previously allocated memory.
 *
 * @param ptr Pointer to memory allocated with bench_malloc()
 */
void bench_free(void *ptr);

/**
 * @brief Get a pointer to the system tick handler
 *
 * This routine returns a pointer to the currently installed system tick
 * handler.
 */
bench_isr_handler_t bench_timer_isr_get(void);

/**
 * @brief Chain in a new system tick handler with the old
 *
 * This routine sets a new system timer handler.
 *
 * @param handler Pointer to the new system tick handler
 */
void bench_timer_isr_set(bench_isr_handler_t handler);

/**
 * @brief Restore the old timer handler and the default tick rate
 *
 * This routine restores both the old timer handler and its default tick rate.
 *
 * @param handler Old timer handler
 */
void bench_timer_isr_restore(bench_isr_handler_t handler);

/**
 * @brief Set the number of microseconds at which to trigger the timer ISR
 *
 * This routine sets the timer interrupt handler to fire after the specified
 * number of microseconds.
 *
 * @param usec Number of microseconds before timer ISR is triggered
 *
 * @return Cycle count at which timer ISR will be triggered
 */
bench_time_t bench_timer_isr_expiry_set(uint32_t usec);

/**
 * @brief Calculate the number of cycles between the trigger and sampling points
 *
 * Given the timer cycle counts for when the timer was triggered and when
 * the timer ISR handler began to execute, this routine calculates the
 * elapsed number of timer cycles. The implementation of this routine is
 * driver specific.
 *
 * @return Number of elapsed timer cycles
 */
bench_time_t bench_timer_cycles_diff(bench_time_t trigger, bench_time_t sample);

/**
 * @brief Get the raw value of the timer cycles
 *
 * @return Raw value of the timer's cycles register
 */
bench_time_t bench_timer_cycles_get(void);

/**
 * @brief Get the number of system timer cycles per second
 *
 * @return Rate at which the timer cycles count
 */
uint32_t bench_timer_cycles_per_second(void);

/**
 * @brief Get the number of timer cycles per tick
 *
 * @return Number of timer cycles per tick
 */
uint32_t bench_timer_cycles_per_tick(void);

/**
 * @brief Sleep current thread
 *
 * @parem msec Time in milliseconds that current thread will sleep
 */
void bench_sleep(uint32_t msec);

#endif /* BENCH_API_H */
