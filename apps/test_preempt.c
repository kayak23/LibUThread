#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "uthread.h"

#define SUM 1000000

static int countA = 0;
static int countB = 0;

static void loopA(void *data)
{
	(void)data;
	preempt_disable();
	while(countA + countB < SUM) fprintf(stdout, "countA: %d\n", countA++);
	preempt_enable();
}

static void loopB(void *data)
{
	(void)data;
	preempt_disable();
	while(countA + countB < SUM) fprintf(stdout, "countB: %d\n", countB++);
	preempt_enable();
}

static void driver(void* data)
{
	(void)data;
	uthread_create(loopA, NULL);
	uthread_create(loopB, NULL);
}

int main(void)
{
	uthread_run(true, driver, NULL);
	fprintf(stdout, "countA: %d, countB: %d\n", countA, countB);
	return EXIT_SUCCESS;
}
