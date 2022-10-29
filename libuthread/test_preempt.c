#include <stdio.h>

#include "uthread.h"

static int num_interrupts;

static void printer(void *arg)
{
	(void)arg;
	fprintf(stderr, "\nPrinter Thread\n");
	uthread_yield();
	fprintf(stderr, "\nBye...\n");
}

static void increment(void *arg)
{
	(void)arg;
	num_interrupts++;
	if(num_interrupts < 1000) {
		uthread_create(increment, NULL);
		if(num_interrupts % 100 == 0)
			uthread_create(printer, NULL);
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
