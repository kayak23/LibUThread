#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
	*(int*)data += 10;
}

/* print queue 
static void q_print(queue_t queue, void *data)
{
	printf("%s\n", (char*)data);
} */

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
	TEST_ASSERT(retval == RET_FAILURE);
}

/* Enqueue NULL */
void test_enqueue_null(void)
{
	int retval;
	queue_t q = queue_create();
	
	fprintf(stderr, "*** TEST enqueue_null ***\n");

	retval = queue_enqueue(q, NULL);
	TEST_ASSERT(retval == RET_FAILURE);
}

/* Enqueue to NULL */
void test_enqueue_to_null(void)
{
	int data = 42;
	int retval;
	
	fprintf(stderr, "*** TEST enqueue_to_null ***\n");

	retval = queue_enqueue(NULL, &data);
	TEST_ASSERT(retval == RET_FAILURE);
}

/* Dequeue from NULL */
void test_dequeue_null(void)
{
	int *ptr;
	int retval;

	fprintf(stderr, "*** TEST dequeue_null ***\n");

	retval = queue_dequeue(NULL, (void**)&ptr);
	TEST_ASSERT(retval == RET_FAILURE);
}

/* Enqueue 50 Dequeue 50 */
void test_enqueue_dequeue_50(void)
{
	int i;
	int j[50];
	bool pass = true;
	queue_t q;

	fprintf(stderr, "*** TEST enqueue_dequeue_50 ***\n");
	
	q = queue_create();
	for (i = 0; i < 50; i++) {
		if(!pass) break;
		j[i] = i;
		int e_ret = queue_enqueue(q, (void*)(j+i));
		pass = (e_ret == RET_SUCCESS);
	}
	for (i = 0; i < 50; i++) {
		if(!pass) break;
		int *ptr;
		int d_ret = queue_dequeue(q, (void**)&ptr);
		pass = (d_ret == RET_SUCCESS && *ptr == j[i]);
	}
	TEST_ASSERT(pass && queue_length(q) == 0);
}

/* Queue Length */
void test_queue_length(void)
{
	int i;
	int j[11];
	bool pass = true;
	queue_t q;

	fprintf(stderr, "*** TEST queue_length ***\n");

	q = queue_create();
	for (i = 0; i < 11; i++) {
		if(!pass) break;
		j[i] = i;
		int e_ret = queue_enqueue(q, (void*)(j+i));
		pass = (e_ret == RET_SUCCESS);
	}
	TEST_ASSERT(pass && queue_length(q) == 11); //queue does not free
}

/* Queue iterate increment */
void test_queue_iterate_increment(void)
{
	queue_t q;
	int i[5] = {0, 1, 2, 3, 4};

	fprintf(stderr, "*** TEST queue_iterate_increment ***\n");

	q = queue_create();

	queue_enqueue(q, (void*)i);
	queue_enqueue(q, (void*)(i+1));
	queue_enqueue(q, (void*)(i+2));
	//queue_enqueue(q, (void*)(i+3));
	//queue_enqueue(q, (void*)(i+4));

	queue_iterate(q, q_increment);
	//queue_iterate(q, q_print);

	TEST_ASSERT(i[0] == 10 && i[1] == 11 && i[2] == 12);
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
	test_dequeue_empty();
	test_enqueue_null();
	test_enqueue_to_null();
	test_dequeue_null();
	test_enqueue_dequeue_50();
	test_queue_length();
	test_queue_iterate_increment();
	test_queue_delete();
	test_delete_nonexistent();

	return 0;
}
