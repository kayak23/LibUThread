#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "uthread.h"
#include "preempt.c"

#define SUM 1000000

static int countA = 0;
static int countB = 0;
static int countC = 0;

static void loopA(void *data)
{
	(void)data;
	preempt_disabe();
	while(countA + countB + countC < SUM) fprintf(stdout, "loop A %d\n", countA++);
	preempt_enable();
}

static void loopB(void *data)
{
	(void)data;
	preempt_disable();
	while(countA + countB + countC < SUM) fprintf(stdout, "loop B %d\n", countB++);
	preempt_enable();
}

static void loopC(void *data)
{
  	(void)data;
	preempt_disable();
  	while(countA + countB + countC < SUM) fprintf(stdout, "loop C %d\n", countC++);
	preempt_enable();
}

static void driver(void* data)
{
	(void)data;
	uthread_create(loopA, NULL);
	uthread_create(loopB, NULL);
  	uthread_create(loopC, NULL);
}

int main(void)
{
	uthread_run(true, driver, NULL);
	fprintf(stdout, "countA: %d, countB: %d, countC: %d\n", countA, countB, countC);
	return EXIT_SUCCESS;
}
