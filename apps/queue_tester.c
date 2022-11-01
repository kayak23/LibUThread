#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "queue.h"

#define LONGEST 36

#ifndef RETVALS
#define RETVALS
#define RET_SUCCESS 0
#define RET_FAILURE -1
#endif

#define TEST_ASSERT(assert)				\
do {									\
	int k;\
	for(k = 0; k < LONGEST-format; k++) fprintf(stdout, " ");\
	if (assert) {						\
		printf("\033[0;32m[PASS]\033[0m\n");				\
	} else	{							\
		printf("\033[0;31m[FAIL]\033[0m\n");				\
		exit(1);						\
	}									\
} while(0)

static int format;

/* Increment */
static void q_increment(queue_t q, void *data)
{
	*(int*)data += 10;
}

static void q_del(queue_t q, void *data)
{
	queue_delete(q, data);
}

/* Ensure create successfully allocates memeory for queue */
void test_create(void)
{
	fprintf(stdout, "*** TEST create *** ... %n", &format);	
	TEST_ASSERT(queue_create() != NULL);
}

/* Ensure destroy returns failure when appropriate */
void test_destroy_error(void)
{
	int item = 0;
	int *dest;
	queue_t q = queue_create();
	queue_enqueue(q, &item);
	fprintf(stdout, "*** TEST destroy nonempty *** ... %n", &format);
	TEST_ASSERT(queue_destroy(q) == RET_FAILURE);
	fprintf(stdout, "*** TEST destroy null *** ... %n", &format);
	TEST_ASSERT(queue_destroy(NULL) == RET_FAILURE);
	queue_dequeue(q, (void**)&dest);
	queue_destroy(q);
}

/* Ensure enqueue returns failure when appropriate */
void test_enqueue_error(void)
{
	int item = 0;
	queue_t q = queue_create();
	fprintf(stdout, "*** TEST enqueue q_null *** ... %n", &format);
	TEST_ASSERT(queue_enqueue(NULL, &item) == RET_FAILURE);
	fprintf(stdout, "*** TEST enqueue d_null *** ... %n", &format);
	TEST_ASSERT(queue_enqueue(q, NULL) == RET_FAILURE);
	queue_destroy(q);
}

/* Ensure dequeue returns failure when appropriate */
void test_dequeue_error(void)
{
	int item = 0;
	int *dest;
	queue_t q = queue_create();
	queue_enqueue(q, &item);
	fprintf(stdout, "*** TEST dequeue q_null *** ... %n", &format);
	TEST_ASSERT(queue_dequeue(NULL, (void**)&dest) == RET_FAILURE);
	fprintf(stdout, "*** TEST dequeue d_null *** ... %n", &format);
	TEST_ASSERT(queue_dequeue(q, NULL) == RET_FAILURE);
	fprintf(stdout, "*** TEST dequeue empty *** ... %n", &format);
	queue_dequeue(q, (void**)&dest);
	TEST_ASSERT(queue_dequeue(q, (void**)&dest) == RET_FAILURE);
	queue_destroy(q);
}

/* Ensure delete returns failure when appropriate */
void test_delete_error(void)
{
	int i[2] = {0, 1};
	queue_t q = queue_create();
	queue_enqueue(q, i);
	queue_enqueue(q, i+1);
	fprintf(stdout, "*** TEST delete q_null *** ... %n", &format);
	TEST_ASSERT(queue_delete(NULL, i) == RET_FAILURE);
	fprintf(stdout, "*** TEST delete d_null *** ... %n", &format);
	TEST_ASSERT(queue_delete(q, NULL) == RET_FAILURE);
	fprintf(stdout, "*** TEST delete no_such *** ... %n", &format);
	queue_delete(q, i);
	TEST_ASSERT(queue_delete(q, i) == RET_FAILURE);
	queue_delete(q, i+1);
	queue_destroy(q);
}

/* Ensure iterate returns failure when appropriate */
void test_iterate_error(void)
{
	int i[2] = {0, 1};
	queue_t q = queue_create();
	queue_enqueue(q, i);
	queue_enqueue(q, i+1);
	fprintf(stdout, "*** TEST iterate q_null *** ... %n", &format);
	TEST_ASSERT(queue_iterate(NULL, q_increment) == RET_FAILURE);
	fprintf(stdout, "*** TEST iterate f_null *** ... %n", &format);
	TEST_ASSERT(queue_iterate(q, NULL) == RET_FAILURE);
	queue_iterate(q, q_del);
	queue_destroy(q);
}

void test_destroy(void)
{
	queue_t q = queue_create();
	fprintf(stdout, "*** TEST destroy *** ... %n", &format);
	TEST_ASSERT(queue_destroy(q) == RET_SUCCESS);
}

void test_enqueue_dequeue(void)
{
	int item = 1;
	int *dest;
	queue_t q = queue_create();
	fprintf(stdout, "*** TEST enqueue *** ... %n", &format);
	TEST_ASSERT(queue_enqueue(q, &item) == RET_SUCCESS);
	fprintf(stdout, "*** TEST dequeue *** ... %n", &format);
	TEST_ASSERT(queue_dequeue(q, (void**)&dest) == RET_SUCCESS && *dest == 1);
	queue_destroy(q);
}

void test_delete_iterate_length(void)
{
	int j;
	int i[3] = {0, 1, 2};
	queue_t q = queue_create();
	for(j = 0; j < sizeof(i)/sizeof(int); j++) queue_enqueue(q, i+j);
	fprintf(stdout, "*** TEST length *** ... %n", &format);
	TEST_ASSERT(queue_length(q) == 3);
	fprintf(stdout, "*** TEST delete *** ... %n", &format);
	TEST_ASSERT(queue_delete(q, i+1) == RET_SUCCESS && queue_length(q) == 2);
	fprintf(stdout, "*** TEST iterate *** ... %n", &format);
	TEST_ASSERT(queue_iterate(q, q_del) == RET_SUCCESS && queue_length == 0);
	queue_destroy(q);
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

int main(void)
{
	/* Error catching */
	test_create();
	test_destroy_error();
	test_enqueue_error();
	test_dequeue_error();
	test_delete_error();
	test_iterate_error();
	
	/* Functionality */
	test_destroy();
	test_enqueue_dequeue();
	test_delete_iterate_length();

	/* Additional tests */
	test_queue_simple();
	test_enqueue_dequeue_50();
	test_queue_length();
	test_queue_iterate_increment();
	test_queue_delete();

	return EXIT_SUCCESS;
}
