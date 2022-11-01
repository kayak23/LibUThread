#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "uthread.h"

#ifndef RETVALS
#define RETVALS
#define RET_SUCCESS 0
#define RET_FAILURE -1
#endif

#define LONGEST 36
#define TEST1_RES 28
#define TEST3_RES 42
#define TEST4_RES 3000

#define TEST_ASSERT(assert)\
do {\
        int k;\
        for(k = 0; k < LONGEST-format; k++) fprintf(stdout, " ");\
        if (assert) {\
                printf(" ... \033[0;32m[PASS]\033[0m\n");\
        } else  {\
                printf(" ... \033[0;31m[FAIL]\033[0m\n");\
        }\
} while(0)

static int format;
static int counter;

void thread1(void *data)
{
	(void)data;
	counter = 15;
	counter = counter*2 + 7 -3*(counter/5);
}

void thread3(void *data)
{
	counter += *(int*)data;
	uthread_yield();
	counter += *(int*)data;
}

void thread2(void *data)
{
	int args = 7;
	counter += *(int*)data;
	fprintf(stdout, "*** TEST uthread create ***%n", &format);
	TEST_ASSERT(uthread_create(thread3, (void*)&args) == RET_SUCCESS);
	uthread_yield();
	counter += *(int*)data;
}

void thread4(void *data)
{
	(void)data;
	if(counter < TEST4_RES-2) {
		counter++;
		uthread_create(thread4, NULL);
		uthread_yield();
		counter++;
	}
}

/* Creates a new thread to modify the global variable counter - EXPECTED VALUE: 28*/
void test_single_thread(void)
{
	fprintf(stdout, "*** TEST single thread ***%n", &format);
	TEST_ASSERT(uthread_run(false, thread1, NULL) == RET_SUCCESS && counter == TEST1_RES);
}

/* Creates multiple threads to modify the global variable counter - EXPECTED VALUE: 42*/
void test_multiple_threads(void)
{
	int retval;
	int args = 14;
	counter = 0;
	retval = uthread_run(false, thread2, (void*)&args);
	fprintf(stdout, "*** TEST multiple threads ***%n", &format);
	TEST_ASSERT(retval == RET_SUCCESS && counter == TEST3_RES);
}

/* Creates many threads (1000) to modify the global variable counter - EXPECTED VALUE: 3000*/
void test_many_threads(void)
{
	counter = 0;
	fprintf(stdout, "*** TEST many threads ***%n", &format);
	TEST_ASSERT(uthread_run(false, thread4, NULL) == RET_SUCCESS && counter == TEST4_RES);
}

/* Notes on testing uthread:
 * There is no parameter-based error catching necessary.
 * uthread_yield() and delete_zombies() are implicitly tested
 * by each of the four tests (two tests are combined in 
 * test_multiple_threads).
 * uthread_create is implicitly tested by each test, but we
 * included an explicit test as a simple demonstration.
 * */
int main(void)
{
	/* Functionality */
	test_single_thread();
	test_multiple_threads();
	test_many_threads();

	return EXIT_SUCCESS;
}
