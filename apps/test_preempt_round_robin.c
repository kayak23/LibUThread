#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "uthread.h"

#define SUM 1000000

static int countA = 0;
static int countB = 0;
static int countC = 0;

static void loopA(void *data)
{
	(void)data;
	while(countA + countB + countC < SUM) fprintf(stdout, "loop A %d\n", countA++);
}

static void loopB(void *data)
{
	(void)data;
	while(countA + countB + countC < SUM) fprintf(stdout, "loop B %d\n", countB++);
}

static void loopC(void *data)
{
  	(void)data;
  	while(countA + countB + countC < SUM) fprintf(stdout, "loop C %d\n", countC++);
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
