#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

typedef struct q_node* node_t;

struct q_node {
	node_t next;
	node_t prev;
	void *data;
};

struct queue {
	/* Phase 1 */
	node_t head;
	node_t tail;
	int len;
};

queue_t queue_create(void)
{
	/* Phase 1 */
	queue_t q = malloc(sizeof(struct queue));
	if (!q) return NULL;

	q->head = NULL;
	q->tail = NULL;
	q->len = 0;
	return q;
}

int queue_destroy(queue_t queue)
{
	/* Phase 1 */
	if (queue == NULL || queue->len != 0) return -1;

	free(queue);
        return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
	/* Phase 1 */
	if (queue == NULL || data == NULL) return -1;

	node_t node = malloc(sizeof(struct q_node));
        node->data = data;
        node->next = NULL;
        node->prev = NULL;

        if (queue_length(queue) == 0) {
                queue->head = node;
                queue->tail = node;
        } else {
                queue->tail->next = node;
                node->prev = queue->tail;
                queue->tail = node;
        }

        queue->len++;
        return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	/* Phase 1 */
        if (queue == NULL || data == NULL || queue_length(queue) == 0) return -1;

        *data = queue->head->data;
        node_t tmp = queue->head;

        queue->head->data = NULL;
        queue->head->next = NULL;
        queue->head->prev = NULL;

        
        queue->head = queue->head->next;

        /* If queue is empty */
        if (queue->head == NULL) {
                queue->tail = NULL;
        } else {
                queue->head->prev = NULL;
        }

        queue->len--;
        free(tmp);
        return 0;
}

int queue_delete(queue_t queue, void *data)
{
	/* Phase 1 */
        

}

int queue_iterate(queue_t queue, queue_func_t func)
{
	/* TODO Phase 1 */
}

int queue_length(queue_t queue)
{
        return queue->len;
	/* TODO Phase 1 */
}

#include <stdio.h>

int main() {
        queue_t queue;
        queue = queue_create();
        char *a  = "aa";
        char *b =  "BB";

        queue_enqueue(queue, a);
        queue_enqueue(queue, b);

        printf("queue head %s\n", (char*) queue->head->data);
        printf("queue tail %s\n", (char*) queue->tail->data);

        queue_delete(queue, "aa");

        printf("queue head %s\n", (char*) queue->head->data);
        printf("queue tail %s\n", (char*) queue->tail->data);

        return 0;
}

