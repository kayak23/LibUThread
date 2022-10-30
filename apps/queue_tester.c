#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

#ifndef RETVALS
#define RETVALS
#define RET_SUCCESS 0
#define RET_FAILURE -1
#endif

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Increment */
static void q_increment(queue_t q, void *data)
{
	*data += 10;
}

/* print queue */
static void q_print(queue_t queue, void *data)
{
	printf("%s\n", (char*)data);
}

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

/* Dequeue empty */
void test_dequeue_empty(void)
{
	int *ptr;
	int retval;
	queue_t q;
	
	fprintf(stderr, "*** TEST dequeue_empty ***\n");

	q = queue_create();
	retval = queue_dequeue(q, (void**)&ptr);
	TEST_ASSRT(retval == RET_FAILURE);
}

/* Enqueue NULL */
void test_enqueue_null(void)
{
	int *ptr;
	int retval;
	queue_t q;
	
	fprintf(stderr, "*** TEST enqueue_null ***\n");

	q = queue_create();
	retval = queue_enqueue(q, NULL);
	TEST_ASSRT(retval == RET_FAILURE);
}

/* Enqueue to NULL */
void test_enqueue_to_null(void)
{
	int data = 42;
	int retval;
	queue_t q;
	
	fprintf(stderr, "*** TEST enqueue_to_null ***\n");

	q = queue_create();
	retval = queue_enqueue(NULL, &data);
	TEST_ASSRT(retval == RET_FAILURE);
}

/* Dequeue from NULL */
void test_dequeue_null(void)
{
	int *ptr;
	int retval;
	queue_t q;
	
	fprintf(stderr, "*** TEST dequeue_null ***\n");

	q = queue_create();
	retval = queue_dequeue(NULL, (void**)&ptr);
	TEST_ASSRT(retval == RET_FAILURE);
}

/* Enqueue 50 Dequeue 50 */
void test_enqueue_dequeue_50(void)
{
	queue_t q;

	fprintf(stderr, "*** TEST enqueue_dequeue_50 ***\n");

	q = queue_create();
	for (int i = 0; i < 50; i++) {
		int e_ret = queue_enqueue(q, i);
		TEST_ASSERT(e_ret == RET_SUCCESS);
	}
	for (int j = 0; j < 50; i++) {
		int d_ret = queue_dequeue(q, (void**)&ptr);
		TEST_ASSERT(d_ret == RET_SUCCESS);
	}
	TEST_ASSERT(queue_length(q) == 0);
}

/* Queue Length */
void test_queue_length(void)
{
	queue_t q;

	fprintf(stderr, "*** TEST queue_length ***\n");

	q = queue_create();
	for (int i = 0; i < 11; i++) {
		int e_ret = queue_enqueue(q, i);
		TEST_ASSERT(e_ret == RET_SUCCESS);
	}
	TEST_ASSERT(queue_length(q) == 11);
}

/* Queue iterate increment */
void test_  queue_iterate_increment(void)
{
	queue_t q;

	fprintf(stderr, "*** TEST queue_iterate_increment ***\n");

	q = queue_create();

	queue_enqueue(q, 0);
	queue_enqueue(q, 1);
	queue_enqueue(q, 2);
	queue_enqueue(q, 3);
	queue_enqueue(q, 4);

	queue_iterate(q, q_increment);
	queue_iterate(q, q_print);
}

/* queue delete */
void test_queue_delete(void) 
{
	queue_t q;

	fprintf(stderr, "*** TEST queue_delete ***\n");

	q = queue_create();
	queue_enqueue(q, "A");
	queue_enqueue(q, "B");
	queue_enqueue(q, "C");

	int retval  = queue_delete(q, "C");
	TEST_ASSERT(queue_length(q) == 2 && retval == RET_SUCCESS);
}

/* Delete nonexistent */
void test_delete_nonexistent(void)
{
	queue_t q;

	fprintf(stderr, "*** TEST delete_nonexistent ***\n");

	q = queue_create();
	queue_enqueue(q, "A");
	queue_enqueue(q, "B");
	queue_enqueue(q, "C");

	int retval  = queue_delete(q, "D");
	TEST_ASSERT(queue_length(q) == 3 && retval == RET_FAILURE);
}

int main(void)
{
	test_create();
	test_queue_simple();


	return 0;
}
