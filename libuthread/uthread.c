#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

#ifndef RET_FAILURE
#define RET_FAILURE -1
#endif

#ifndef RET_SUCCESS
#define RET_SUCCESS 0
#endif

#define T_RUNNING 	1
#define T_READY 	2
#define T_BLOCKED 	3
#define T_EXITED	4

/* Changed from stack_t to void */
struct uthread_tcb {
	/* TODO Phase 2 */
	uthread_ctx_t *context;
	void *stack; //where is this defined? is it from signal.h? uthread_ctx_t has a stack member
	int state;
};

static queue_t thread_queue;
static struct uthread_tcb *idle_thread; // changed from queue_t to uthread_tcb
static struct uthread_tcb *curr_thread;

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/4 */
	return curr_thread;
}

void uthread_yield(void)
{
	/* TODO Phase 2 USE LOCKS HERE IN FUTURE (PROBABLY) */
	if (curr_thread->state != T_EXITED || curr_thread->state != T_BLOCKED)
		curr_thread->state = T_READY;
	queue_enqueue(thread_queue, curr_thread);
	queue_dequeue(thread_queue, &curr_thread);
	curr_thread->state = T_RUNNING;
	uthread_ctx_switch(thread_queue->tail->context, curr_thread->context);
}

void uthread_exit(void)
{
	/* TODO Phase 2 USE LOCKS HERE IN FUTURE (PROBABLY) */
        curr_thread->state = T_EXITED;
        uthread_yeild();
}

int uthread_create(uthread_func_t func, void *arg)
{
        uthread_tcb *new_thread;
        if ((new_thread = malloc(sizeof(struct uthread_tcb))) == NULL)
		return RET_FAILURE;
        if ((new_thread->context = malloc(sizeof(uthread_ctx_t))) == NULL)
		return RET_FAILURE;
        if ((new_thread->stack = uthread_ctx_alloc_stack()) == NULL)
		return RET_FAILURE;
        if (uthread_ctx_init(new_thread->context, new_thread->stack, func, arg) < 0)
                return RET_FAILURE;
        new_thread->state = T_READY;
        queue_enqueue(thread_queue, new_thread);
        return RET_SUCCESS;
}

static void delete_zombies(queue_t queue, void *thread)
{
	if (thread->state == T_EXITED)
	{
		queue_delete(thread_queue, thread);
		free(thread->context);
		uthread_ctx_destroy_stack(thread->stack);
		free(thread);
	}

}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/*  Phase 2 */

	/* build the thread queue */
	if ((thread_queue = queue_create()) == NULL) 
		return RET_FAILURE;
	/* Build the idle thread */
	if(uthread_create(NULL, NULL) < 0)
		return RET_FAILURE;
	queue_dequeue(thread_queue, &curr_thread);
	curr_thread->state = T_RUNNING;
	/* Build the initial thread */
	if(uthread_create(func, arg) < 0)
		return RET_FAILURE;
        /* 
         * When there are no more threads which are ready to run
         * in the system, stops the idle loop and returns.
         * Or simply yields to next available thread.
         * 
         * (It could also deal with threads that
         * reached completion and destroys
         * their associated TCB.)
         */
	do {
		queue_iterate(thread_queue, delete_zombies);
	} while (queue_length(thred_queue));

        uthread_ctx_destroy_stack(idle_thread->stack);
	free(idle_thread->context);
        free(idle_thread);

        return 0;
}

/* for both of these guys, use locks in the future */

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
	uthread_yield(); //I don't have justification for this but it makes sense intuitively
}

