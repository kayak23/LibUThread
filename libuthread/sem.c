#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef QUEUE_H
#define QUEUE_H
#include "queue.h"
#endif

#ifndef SEM_H
#define SEM_H
#include "sem.h"
#endif

#ifndef PRIV_H
#define PRIV_H
#include "private.h"
#endif

#ifndef RETVALS
#define RETVAlS
#define RET_SUCCESS 0
#define RET_FAILURE -1
#endif

typedef struct uthread_tcb* uthread_t;

/*
 * Semaphore data structure
 * - count: internal semaphore count
 * - q_blocked: list of blocked threads
 */
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
        queue_destroy(sem->q_blocked);
        free(sem);
        return RET_SUCCESS;
}

int sem_down(sem_t sem)
{
        // we should probably make this a critical section.

        if (sem == NULL)
                return RET_FAILURE;

	preempt_disable();
        /* If sem count is zero, we block ourselves */
        while (sem->count == 0) {
                uthread_t self = uthread_current();
                queue_enqueue(sem->q_blocked, self);
		preempt_enable();
                uthread_block();
        }

        sem->count--;
	preempt_enable();

        return RET_SUCCESS;
}

int sem_up(sem_t sem)
{
        // we should probably make this a critical section

        if (sem == NULL)
                return RET_FAILURE;
        
	preempt_disable();
        /* unblock the oldest thread in q_blocked */
        if (queue_length(sem->q_blocked) != 0) {
                uthread_t oldest;
                queue_dequeue(sem->q_blocked, (void**) &oldest);
                preempt_enable();
		uthread_unblock(oldest);
        }
        sem->count++;
	preempt_enable();
        return RET_SUCCESS;
}
