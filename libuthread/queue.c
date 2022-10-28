#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define RET_SUCCESS 0
#define RET_FAILURE -1

typedef struct q_node* node_t;

struct q_node {
        node_t next;
        node_t prev; //TODO: eliminate if unused
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
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
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

        queue->head->data = NULL;
        queue->head->next = NULL;
        queue->head->prev = NULL;
        node_t new_head = queue->head->next;
        free(queue->head);
        queue->head = new_head;
        
        /* If queue is empty */
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

#include <stdio.h>

static void q_print(queue_t queue, void *data)
{
        printf("%s\n", (char*)data);
}

static void q_delete(queue_t queue, void *data)
{
        queue_delete(queue, data);
}

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
        queue_enqueue(queue, a);
        queue_enqueue(queue, a);
        queue_iterate(queue, q_print);
        queue_iterate(queue, q_delete);
        queue_iterate(queue, q_print);
        queue_enqueue(queue, "HelloWorld");
        printf("enqueued\n");
        void **collector = (void**)&a;
        void **trash = NULL;
        int retval = queue_dequeue(queue, trash);
        printf("dequeued\n");
        if(retval == -1)
                printf("dequeue failed\n");
        printf("Collected value: %s\n", *(char**)collector);
        printf("Done!\n");
        return 0;
}

