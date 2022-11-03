#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "uthread.h"

#define LONGEST 36

static int interrupts;
static int format;
struct timeval t1, t2;
double t_elapsed;

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
        gettimeofday(&t1, NULL);

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
	
        gettimeofday(&t2, NULL);
        t_elapsed = (t2.tv_sec - t1.tv_sec) * 1000.0;
        t_elapsed += (t2.tv_usec - t1.tv_usec) / 1000.0;
        fprintf(stdout, "\nTime: %f ms ... \033[32m[PASS]\033[0m\n", t_elapsed);
}

int main(void)
{
	uthread_run(true, greedy, NULL);
	return EXIT_SUCCESS;
}
