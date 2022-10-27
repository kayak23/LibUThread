#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

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

static struct queue* thread_queue;
static struct uthread_tcb *curr_thread;

struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/4 */
	return curr_thread;
}

void uthread_yield(void)
{
	/* TODO Phase 2 USE LOCKS HERE IN FUTURE (PROBABLY) */
	//fprintf(stderr, "[yield] Beginning yield procedure\n");
	struct uthread_tcb *tail = curr_thread;
	if (curr_thread->state != T_EXITED && curr_thread->state != T_BLOCKED)
		curr_thread->state = T_READY;
	//fprintf(stderr, "[yield] State codes set\n");
	queue_enqueue(thread_queue, curr_thread);
	//fprintf(stderr, "[yield] Thread enqueued\n");
	queue_dequeue(thread_queue, (void**)&curr_thread);
	//fprintf(stderr, "[yield] Thread dequeued\n");
	curr_thread->state = T_RUNNING;
	uthread_ctx_switch(tail->context, curr_thread->context);
	//fprintf(stderr, "[yield] Yield procedure completed\n");
}

void uthread_exit(void)
{
	/* TODO Phase 2 USE LOCKS HERE IN FUTURE (PROBABLY) */
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
        queue_enqueue(thread_queue, new_thread);
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
	/*  Phase 2 */
	if(preempt || !preempt)
		fprintf(stderr, "Hello!\n");
	/* build the thread queue */
	if ((thread_queue = queue_create()) == NULL) 
		return RET_FAILURE;
	/* Build the idle thread */
	if(uthread_create(NULL, NULL) < 0)
		return RET_FAILURE;
	queue_dequeue(thread_queue, (void**)&curr_thread);
	curr_thread->state = T_RUNNING;
	/* Build the initial thread */
	if(uthread_create(func, arg) < 0)
		return RET_FAILURE;
        /* Begin the idle loop */
	//int i = 0;
	do {
		//fprintf(stderr, "[idle] Entering cycle %d\n", i);
		queue_iterate(thread_queue, delete_zombies);
		uthread_yield();
		//fprintf(stderr, "[idle] Exiting cycle %d\n", i++);
	} while (queue_length(thread_queue));

        uthread_ctx_destroy_stack(curr_thread->stack);
	queue_destroy(thread_queue);
	free(curr_thread->context);
        free(curr_thread);

        return RET_SUCCESS;
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

