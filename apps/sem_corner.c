#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sem.h"
#include "uthread.h"

#define EXP_RESULT "BCCA"
#define LONGEST 36
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

static sem_t sem;
static char *str = "----";
static int format;
static int i;

static void threadC(void *data)
{
	(void)data;
	sem_down(sem);
	str[i++] = 'C';
	uthread_yield();
	str[i++] = 'C';
	sem_up(sem);
}

static void threadB(void *data)
{
	(void)data;
	sem_up(sem);
	str[i++] = 'B';
}

static void threadA(void *data)
{
	(void)data;
	sem_down(sem);
	str[i++] = 'A';
}

void thread1(void *data)
{
	(void)data;
	uthread_create(threadA, NULL);
	uthread_create(threadB, NULL);
	uthread_create(threadC, NULL);
}

int main(void)
{
	sem = sem_create(0);
	fprintf(stdout, "*** TEST sem corner case ***%n", &format);
	uthread_run(false, thread1, NULL);
	TEST_ASSERT(strcmp(str, EXP_RESULT));
	sem_destroy(sem);

	return EXIT_SUCCESS;
}
