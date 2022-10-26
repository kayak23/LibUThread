#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

#define T_RUNNING 	1
#define T_READY 	2
#define T_BLOCKED 	3
#define T_EXITED	4

struct uthread_tcb {
	/* TODO Phase 2 */
	uthread_ctx_t context;
	stack_t *stack;
	int state;
};

static queue_t thread_queue;
static queue_t idle_thread;
static uthread_tcb *curr_thread;

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/4 */
	return curr_thread;
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
	
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
        curr_thread->state = T_EXITED;
        uthread_yeild();
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* Phase 2 */
        uthread_tcb *new_thread = malloc(sizeof(uthread_tcb));
        if (new_thread == NULL) return -1;

        new_thread->context = malloc(sizeof(uthread_ctx_t));
        if (new_thread->context == NULL) return -1;

        new_thread->stack = malloc(sizeof(stack_t));
        if (new_thread->stack == NULL) return -1;

        if (uthread_ctx_init(new_thread->context, new_thread->stack, func, arg) < 0)
                return 1;

        new_thread->state = T_READY;
        queue_enqueue(thread_queue, new_thread);

        return 0;
}

int uthread_run(uthread_func_t func, void *arg)
{
	/*  Phase 2 */

	/* 
         * register the so-far single execution
	 * flow of the application as the idle thread 
	 * that the library can later schedule for execution 
	 * like any other threads.
	 */
        thread_queue = queue_create();
	if (!thread_queue) return 1;

        /*
         * Create a new thread, the initial thread, 
         * as specified by the arguments of the function.
         */
	idle_thread->context = malloc(sizeof(uthread_tcb));
	if (!idle_threads) return -1;

	curr_thread = idle_thread;
	curr_thread->state = T_RUNNING;

        /* 
         * When there are no more threads which are ready to run
         * in the system, stops the idle loop and returns.
         * Or simply yields to next available thread.
         * 
         * (It could also deal with threads that
         * reached completion and destroys
         * their associated TCB.)
         */
	if (uthread_create(funct, arg) < 0) return -1;

	do {
		uthread_yield();
	} while (queue_length(thred_queue));

        uthread_ctx_destroy_stack(idle_thread->stack);
	free(idle_thread->context);
        free(idle_thread);

        return 0;
}

void uthread_block(void)
{
	/* TODO Phase 4 */
	curr_thread->state = T_BLOCKED;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 4 */
	uthread->state = T_READY;
	queue_enqueue(thread_queue, uthread);
	
}

