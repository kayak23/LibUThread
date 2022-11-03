#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "uthread.h"

#define LONGEST 36
/*
static int interrupts;
static int format;
struct timeval t1, t2;
double t_elapsed;
*/
/*
static void interrupt(void *arg)
{
	(void)arg;
	interrupts = 1;
}*/

/* greedy never yields so it will stay in the loop
 * until interrupt is scheduled. This can only happen
 * if the virtual timer forcibly yields greedy.
 * */
/*static void greedy(void *arg)
{
        gettimeofday(&t1, NULL);

	(void)arg;
	int i;
	int first;
	fprintf(stdout, "*** TEST Preemption ***%n", &format);
	fflush(stdout);
	while (interrupts != 0) {
		
	}
	for (i = 0; i < LONGEST-format; i++) fprintf(stdout, " ");
        fprintf(stdout, " ... \033[32m[PASS]\033[0m\n", t_elapsed);
}*/

static void loop1(void *data)
{
	(void)data;
	while(true) fprintf(stdout, "loop A\n");
}

static void loop2(void *data)
{
	(void)data;
	while(true) fprintf(stdout, "loop B\n");
}

static void driver(void* data)
{
	(void)data;
	uthread_create(loop1, NULL);
	uthread_create(loop2, NULL);
}

int main(void)
{
	uthread_run(true, driver, NULL);
	return EXIT_SUCCESS;
}
