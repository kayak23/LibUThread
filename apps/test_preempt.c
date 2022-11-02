#include <stdio.h>
#include <stdlib.h>

#include "uthread.h"

static int num_interrupts;

static void increment(void *arg)
{
	(void)arg;
	num_interrupts++;
	if(num_interrupts < 1000)
		uthread_create(increment, NULL);
}

static void greedy(void *arg)
{
	(void)arg;
	int chars, i;
	uthread_create(increment, NULL);
	fprintf(stdout, "I have been interrupted ");
	while(num_interrupts < 1000)
	{
		fprintf(stdout, "%d times.%n", num_interrupts, &chars);
		for(i = 0; i < chars; i++) fprintf(stdout, "\b");
	}
	fprintf(stdout, "\n");
}

int main(void)
{
	uthread_run(true, greedy, NULL);
	return EXIT_SUCCESS;
}
