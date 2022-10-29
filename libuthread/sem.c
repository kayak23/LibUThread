#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

#ifndef RETVALS
#define RETVAlS
#define RET_SUCCESS 0
#define RET_FAILURE -1
#endif

typedef struct uthread_tcb* uthread_t;

struct semaphore {
	int count;
	queue_t q_blocked;
};

sem_t sem_create(size_t count)
{
	/* Allocate space for semaphore*/
	fprintf(stderr, "[sem_create] Creating a semaphore...");
	sem_t sem = malloc(sizeof(struct semaphore));
	if (sem == NULL)
		return NULL;
	
	/* Initiate members of sem */
	sem->count = count;
	sem->q_blocked = queue_create();
	fprintf(stderr, "...semaphore created successfully.\n");
	return sem;
}

int sem_destroy(sem_t sem)
{
	fprintf(stderr, "[sem_destory] Destroying a semaphore...");
	if (sem == NULL || queue_length(sem->q_blocked) != 0)
		return RET_FAILURE;

	/* destroy q_blocked */
	queue_destroy(sem->q_blocked);

	free(sem);
	frpintf(stderr, "...semaphore destroyed successfully.\n");
	return RET_SUCCESS;
}

int sem_down(sem_t sem)
{	
	/*
 	 * spinlock_lock(sem->lock);
     * while (sem->count == 0) {
     *     // Block self //
     * }
     * sem->count -= 1;
     * spinlock_unlock(sem->lock);
	 */
	fprintf(stderr, "[sem_down] Down procedure invoked.\n");
	if (sem == NULL)
		return RET_FAILURE;

	/* If sem count is zero, we block ourselves */
	if (sem->count == 0) {
		fprintf(stderr, "[sem_down] out of resources; blocking thread\n");
		uthread_t self = uthread_current();
		queue_enqueue(sem->q_blocked, self);
		uthread_block();
	}

	sem->count--;
	fprintf(stderr, "[sem_down] Down procedure completed.\n");
	return RET_SUCCESS;
}

int sem_up(sem_t sem)
{
	/*
	 * spinlock_lock(sem->lock);
     * sem->count += 1;
     * / Wake up first in line /
     * / (if any)              /
     * ...
     * spinlock_unlock(sem->lock);
	 */
	fprintf(stderr, "[sem_up] Up procedure invoked.\n");
	if (sem == NULL)
		return RET_FAILURE;
	
	if (queue_length(sem->q_blocked) != 0) {
		fprintf(stderr, "[sem_up] There are blocked threads. Unlocking...");
		uthread_t oldest;
		queue_dequeue(sem->q_blocked, (void**) &oldest);
		uthread_unblock(oldest);
		fprintf(stderr, "...unblocking successful.\n");
	}
	fprintf(stderr, "[sem_up] Up procedure completed.\n");
	sem->count++;
	return RET_SUCCESS;
}

