#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "uthread.h"
#include "sem.h"

#define SUM 1000000

sem_t sem;

static int countA = 0;
static int countB = 0;
static int countC = 0;

static void loopA(void *data)
{
	(void)data;
	while(countA + countB + countC < SUM) {
                sem_down(sem);
                fprintf(stdout, "loop A %d\n", countA++);
                sem_up(sem);
        }
}

static void loopB(void *data)
{
	(void)data;
	while(countA + countB + countC < SUM) {
                sem_down(sem);
                fprintf(stdout, "loop B %d\n", countB++);
                sem_up(sem);
        }
}

static void loopC(void *data)
{
        (void)data;
        while(countA + countB + countC < SUM) {
                sem_down(sem);
                fprintf(stdout, "loop C %d\n", countC++);
                sem_up(sem);
        }
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
        sem = sem_create(1);
	uthread_run(true, driver, NULL);
	fprintf(stdout, "countA: %d, countB: %d, countC: %d\n", countA, countB, countC);
	return EXIT_SUCCESS;
}
