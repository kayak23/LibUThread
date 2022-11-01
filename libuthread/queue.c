#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h> //remove after debug version

#ifndef QUEUE_H
#define QUEUE_H
#include "queue.h"
#endif

#ifndef RETVALS
#define RETVALS
#define RET_SUCCESS 0
#define RET_FAILURE -1
#endif

typedef struct q_node* node_t;

struct q_node {
	node_t next;
	node_t prev;
	void *data;
};

struct queue {
	node_t head;
	node_t tail;
	int len;
};

queue_t queue_create(void)
{
	queue_t q;
	if((q = malloc(sizeof(struct queue))) == NULL)
		return NULL;
	q->head = NULL;
	q->tail = NULL;
	q->len = 0;
	return q;
}

int queue_destroy(queue_t queue)
{
	if (queue == NULL || queue->len != 0) 
		return RET_FAILURE;
	free(queue);
        return RET_SUCCESS;
}

int queue_enqueue(queue_t queue, void *data)
{
	node_t node;
	if (queue == NULL || data == NULL) 
		return RET_FAILURE;
	if((node = malloc(sizeof(struct q_node))) == NULL)
		return RET_FAILURE;
	//fprintf(stderr, "[enqueue] Beginning enqueue procedure\n");
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
	//fprintf(stderr, "[enqueue] Values init'd\n");
        if (queue->len == 0) {
                queue->head = node;
                queue->tail = node;
        } else {
		/*if(queue->tail == NULL) {
			fprintf(stderr, "[enqueue] Error! Null tail pointer!\n");
			return RET_FAILURE;
		}*/
		//fprintf(stderr, "[enqueue] Setting queue params...");
                queue->tail->next = node;
		//fprintf(stderr, "tail->next done...");
                node->prev = queue->tail;
		//fprintf(stderr, "node->prev done...");
                queue->tail = node;
		//fprintf(stderr, "queue->tail done\n");
        }
        queue->len++;
	//fprintf(stderr, "[enqueue] Enqueue procedure completed\n");
        return RET_SUCCESS;
}

int queue_dequeue(queue_t queue, void **data)
{
        if (queue == NULL || data == NULL || queue_length(queue) == 0) 
		return RET_FAILURE;

	*data = queue->head->data;
	node_t new_head = queue->head->next;

        queue->head->data = NULL;
        queue->head->next = NULL;
        queue->head->prev = NULL;
	free(queue->head);
        queue->head = new_head;
	
        /* If queue is empty */
        if (queue->head == NULL)
                queue->tail = NULL;
        else
                queue->head->prev = NULL;

        queue->len--;
	//fprintf(stderr, "[dequeue] Queue size is now %d\n", queue->len);
        return RET_SUCCESS;
}

int queue_delete(queue_t queue, void *data)
{
	if (queue == NULL || data == NULL)
		return RET_FAILURE;
	node_t iter = queue->head;
	while (iter != NULL) {
		if (iter->data == data) {
			if (iter != queue->tail)
				iter->next->prev = iter->prev;
			if (iter != queue->head)
				iter->prev->next = iter->next;
			if (iter == queue->head)
				queue->head = iter->next;
			if (iter == queue->tail)
				queue->tail = iter->prev;
			iter->next = NULL;
			iter->prev = NULL;
			iter->data = NULL;
			queue->len--;
			free(iter);
			return RET_SUCCESS;
		}
		iter = iter->next;
	}
	return RET_FAILURE;
}

int queue_iterate(queue_t queue, queue_func_t func)
{
	if (queue == NULL || func == NULL)
		return RET_FAILURE;
	node_t iter = queue->head;
	node_t next;
	while (iter != NULL) {
		next = iter->next;
		func(queue, iter->data);
		iter = next;
	}
	return RET_SUCCESS;
}

int queue_length(queue_t queue)
{
        return queue->len;
}

/*
static void q_print(queue_t queue, void *data)
{
	printf("%s\n", (char*)data);
}

static void q_delete(queue_t queue, void *data)
{
	queue_delete(queue, data);
}*/
