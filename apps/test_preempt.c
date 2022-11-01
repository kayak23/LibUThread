#include <stdio.h>

#include "uthread.h"
#include "private.h"

static int num_interrupts;

static void increment(void *arg)
{
	(void)arg;
	//preempt_disable();
	num_interrupts++;
	if(num_interrupts < 1000) {
		uthread_create(increment, NULL);
	}
}

static void greedy(void *arg)
{
	(void)arg;
	uthread_create(increment, NULL);

	int chars;
	char *str = "I have been interrupted ";
	fprintf(stderr, "%s", str);
	while(num_interrupts < 1000)
	{
		fprintf(stderr, "%d times.%n", num_interrupts, &chars);
		int i;
		for(i = 0; i < chars; i++) fprintf(stderr, "\b");
	}
	fprintf(stderr, "\n");
}

int main(void)
{
	uthread_run(true, greedy, NULL);
	return 0;
}
