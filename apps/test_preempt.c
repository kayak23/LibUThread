#include <stdio.h>
#include <stdlib.h>

#include "uthread.h"

#define LONGEST 36

static int interrupts;
static int format;

static void interrupt(void *arg)
{
	(void)arg;
	interrupts = 1;
}

/* greedy never yields so it will stay in the loop
 * until interrupt is scheduled. This can only happen
 * if the virtual timer forcibly yields greedy.
 * */
static void greedy(void *arg)
{
	(void)arg;
	int i;
	int first;
	fprintf(stdout, "*** TEST Preemption ***%n", &format);
	fflush(stdout);
	while (interrupts != 0) {
		if (first == 0) {
			first = 1;
			uthread_create(interrupt, NULL);
		}
	}
	for (i = 0; i < LONGEST-format; i++) fprintf(stdout, " ");
	fprintf(stdout, " ... \033[32m[PASS]\033[0m\n");
}

int main(void)
{
	uthread_run(true, greedy, NULL);
	return EXIT_SUCCESS;
}
