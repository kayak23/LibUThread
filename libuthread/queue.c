#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

#define RET_SUCCESS 0
#define RET_FAILURE -1

/* Struct queue is implemented as a singly linked list.
 * data is the void pointer to the contents of the item
 * next points to the next item in the queue
 * head points to the front of the queue
 * tail points to the tail of the queue
 */
struct queue {
	void *data;
	queue_t next;
	queue_t head;
	queue_t tail;
};

queue_t queue_create(void)
{
	queue_t queue;
	if((queue = malloc(sizeof(struct queue))) == NULL)
		return NULL;
	queue->next = NULL;
	queue->data = NULL;
	queue->head = queue;
	queue->tail = queue;
	return queue;
}

int queue_destroy(queue_t queue)
{
	/* Either the queue DNE or the queue has data */
	if(queue == NULL || queue->head->data != NULL)
		return RET_FAILURE;
	free(queue);
	return RET_SUCCESS;
}

int queue_enqueue(queue_t queue, void *data)
{
	if(queue == NULL || data == NULL)
		return RET_FAILURE;
	if((queue->tail->next = malloc(sizeof(struct queue))) == NULL)
		return RET_FAILURE;
	queue->data = data;
	queue->tail = queue->tail->next;
	queue->tail->next->data = NULL;
	queue->tail->next->next = NULL;
	queue->tail->next->head = queue->head;
	queue->tail->next->tail = queue->tail->next;
	return RET_SUCCESS;
}

int queue_dequeue(queue_t queue, void **data)
{
	if(queue == NULL || data == NULL)
		return RET_FAILURE;
	if(queue->head->data == NULL)
		return RET_FAILURE;
	*data = queue->head->data;
	queue->head = queue->next;
}

int queue_delete(queue_t queue, void *data)
{
	/* TODO Phase 1 */
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
	/* TODO Phase 1 */
}

