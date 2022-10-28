#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

#ifndef RET_FAILURE
#define RET_FAILURE -1
#endif

#ifndef RET_SUCCESS
#define RET_SUCCESS 0
#endif

struct semaphore {
	int count;
	queue_t q_blocked;
};

sem_t sem_create(size_t count)
{
	/* Allocate space for semaphore*/
	sem_t sem = malloc(sizeof(struct semaphore));
	if (sem == NULL)
		return NULL;
	
	/* Initiate members of sem */
	sem->count = count;
	sem->q_blocked = queue_create();

	return sem;
}

int sem_destroy(sem_t sem)
{
	if (sem == NULL || queue_length(sem->q_blocked) != 0)
		return RET_FAILURE;

	/* destroy q_blocked */
	queue_destroy(sem->q_blocked);

	free(sem);
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
	
	if (sem == NULL)
		return RET_FAILURE;

	/* If sem count is zero, we block ourselves */
	if (sem->count == 0) {
		uthead_t self = uthread_current();
		queue_enqueue(sem->q_blocked, self);
		uthread_block();
	}

	sem->count--;
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
	if (sem == NULL)
		return RET_FAILURE;
	
	if (queue_length(sem->q_blocked) != 0) {
		uthread_t oldest;
		queue_dequeue(sem->q_blocked, (void**) &oldest);
		uthread_unblock(oldest);
	}

	sem->count++;
	return RET_SUCCESS;
}

