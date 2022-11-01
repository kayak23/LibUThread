#include <stdio.h>
#include <stdlib.h>

#include "sem.h"
#include "uthread.h"

static sem_t sem;

static void threadC(void *data)
{
	(void)data;
	sem_down(sem);
	fprintf(stdout, "[C] I executed\n");
	uthread_yield();
	fprintf(stdout, "[C] I executed again\n");
	sem_up(sem);
}

static void threadB(void *data)
{
	(void)data;
	sem_up(sem);
	fprintf(stdout, "[B] I executed\n");
}

static void threadA(void *data)
{
	(void)data;
	sem_down(sem);
	fprintf(stdout, "[A] I executed\n");
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
	uthread_run(false, thread1, NULL);

	return EXIT_SUCCESS;
}
