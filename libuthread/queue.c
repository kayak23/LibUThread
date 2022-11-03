#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

/*
 * Implemented queue as a double linked list.
 * Each node contains a void data pointer.
 * Queue struct keeps track of head to dequeue in O(1)
 * queue struct keeps track of tail to enqueue in O(1)
 */
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
        /* Allocate memroy for queue in the heap */
        if((q = malloc(sizeof(struct queue))) == NULL)
                return NULL;

        /* Initialize members of queueu with initial values */
        q->head = NULL;
        q->tail = NULL;
        q->len = 0;
        
        return q;
}

int queue_destroy(queue_t queue)
{       
        /* Error if the queue is not empty */
        if (queue == NULL || queue->len != 0) 
                return RET_FAILURE;
        
        free(queue);
        return RET_SUCCESS;
}

int queue_enqueue(queue_t queue, void *data)
{
        if (queue == NULL || data == NULL) 
                return RET_FAILURE;

        /* Allocate new node in the heap. Initiallize its members. */
        node_t node;
        if((node = malloc(sizeof(struct q_node))) == NULL)
                return RET_FAILURE;
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
        
        /* If this is the only node, its is both head & tail */
        if (queue->len == 0) {
                queue->head = node;
                queue->tail = node;
        } else {
                queue->tail->next = node;
                node->prev = queue->tail;
                queue->tail = node;
        }
        queue->len++;

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

        if (queue->head == NULL)
                queue->tail = NULL;
        else
                queue->head->prev = NULL;

        queue->len--;
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
