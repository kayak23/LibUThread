#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "uthread.h"
#include "sem.h"

#define SUM 1000000

sem_t sem;

static int countA = 0;
static int countB = 0;

static void loopA(void *data)
{
	(void)data;
	while(countA + countB < SUM) {
                sem_down(sem);
                fprintf(stdout, "countA: %d\n", countA++);
                sem_up(sem);
        }
}

static void loopB(void *data)
{
	(void)data;
	while(countA + countB < SUM) {
                sem_down(sem);
                fprintf(stdout, "countB: %d\n", countB++);
                sem_up(sem);
        }
}

static void driver(void* data)
{
	(void)data;
	uthread_create(loopA, NULL);
	uthread_create(loopB, NULL);
}

int main(void)
{
        sem = sem_create(1);
	uthread_run(true, driver, NULL);
	fprintf(stdout, "countA: %d, countB: %d\n", countA, countB);
	return EXIT_SUCCESS;
}
