#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#ifndef PRIV_H
#define PRIV_H
#include "private.h"
#endif

#ifndef UTHD_H
#define UTHD_H
#include "uthread.h"
#endif

#ifndef QUEUE_H
#define QUEUE_H
#include "queue.h"
#endif

#ifndef RETVALS
#define RETVALS
#define RET_SUCCESS 0
#define RET_FAILURE -1
#endif

#define T_RUNNING 	1
#define T_READY 	2
#define T_BLOCKED 	3
#define T_EXITED	4

/* Changed from stack_t to void */
struct uthread_tcb {
	uthread_ctx_t *context;
	void *stack;
	int state;
};

static struct queue* thread_queue;
static struct uthread_tcb *curr_thread;

struct uthread_tcb *uthread_current(void)
{
	return curr_thread;
}

void uthread_yield(void)
{
	preempt_disable();
	//fprintf(stderr, "[yield] Beginning yield procedure\n");
	struct uthread_tcb *tail = curr_thread;
	if (curr_thread->state == T_RUNNING)
		curr_thread->state = T_READY;
	//fprintf(stderr, "[yield] State codes set\n");
	do {
		queue_enqueue(thread_queue, curr_thread);
		//fprintf(stderr, "[yield] Thread enqueued\n");
		queue_dequeue(thread_queue, (void**)&curr_thread);
	} while (curr_thread->state != T_READY);
	//fprintf(stderr, "[yield] Thread dequeued\n");
	//if (curr_thread == NULL)
	//	fprintf(stderr, "[yield] Error! curr_thread is null!\n");
	//fprintf(stderr, "[yield] Switching context\n");
	curr_thread->state = T_RUNNING;
	preempt_enable(); //always the possibility that there a SIGVTALRM is received here *skull emoji*
	uthread_ctx_switch(tail->context, curr_thread->context);
	//fprintf(stderr, "[yield] Yield procedure completed\n");
}

void uthread_exit(void)
{
	//fprintf(stderr, "[exit] A thread was terminated\n");
        curr_thread->state = T_EXITED;
        uthread_yield();
}

int uthread_create(uthread_func_t func, void *arg)
{
	//fprintf(stderr, "[ucreate] Creating a thread\n");
        struct uthread_tcb *new_thread;
        if ((new_thread = malloc(sizeof(struct uthread_tcb))) == NULL)
		return RET_FAILURE;
        if ((new_thread->context = malloc(sizeof(uthread_ctx_t))) == NULL)
		return RET_FAILURE;
        if ((new_thread->stack = uthread_ctx_alloc_stack()) == NULL)
		return RET_FAILURE;
        if (uthread_ctx_init(new_thread->context, new_thread->stack, func, arg) < 0)
                return RET_FAILURE;
        new_thread->state = T_READY;
	preempt_disable();
        queue_enqueue(thread_queue, new_thread);
	preempt_enable();
	//fprintf(stderr, "[ucreate] Created a thread\n");
        return RET_SUCCESS;
}

static void delete_zombies(queue_t queue, void *data)
{
	//fprintf(stderr, "[DZ] Deleting zombies\n");
	struct uthread_tcb *thread = data;
	//fprintf(stderr, "[DZ] Thread code: %d\n", thread->state);
	if (thread->state == T_EXITED)
	{
		//fprintf(stderr, "[DZ] Found a zombie!\n");
		queue_delete(queue, thread);
		free(thread->context);
		uthread_ctx_destroy_stack(thread->stack);
		free(thread);
		//fprintf(stderr, "[DZ] Beheaded a zombie.\n");
	}
	//fprintf(stderr, "[DZ] Finished deleting zombies\n");

}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* build the thread queue */
	if ((thread_queue = queue_create()) == NULL) 
		return RET_FAILURE;
	/* Build the idle thread */
	if (uthread_create(NULL, NULL) < 0)
		return RET_FAILURE;
	queue_dequeue(thread_queue, (void**)&curr_thread);
	curr_thread->state = T_RUNNING;
	/* Build the initial thread */
	if (uthread_create(func, arg) < 0)
		return RET_FAILURE;
        /* Begin the idle loop */
	//int i = 0;
	preempt_start(preempt);
	while (queue_length(thread_queue)) {
		//fprintf(stderr, "[idle] Entering cycle %d\n", i);
		preempt_disable();
		queue_iterate(thread_queue, delete_zombies);
		preempt_enable();
		uthread_yield();
		//fprintf(stderr, "[idle] Exiting cycle %d\n", i++);
	}
	/* cleanup */
	preempt_stop();
        uthread_ctx_destroy_stack(curr_thread->stack);
	queue_destroy(thread_queue);
	free(curr_thread->context);
        free(curr_thread);

        return RET_SUCCESS;
}

void uthread_block(void)
{
	curr_thread->state = T_BLOCKED;
	uthread_yield();
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = T_READY;
}
