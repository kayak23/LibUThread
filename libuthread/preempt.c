#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#ifndef PRIV_H
#define PRIV_H
#include "private.h"
#endif

#ifndef UTHD_H
#define UTHD_H
#include "uthread.h"
#endif

#ifndef RETVALS
#define RETVALS
#define RET_SUCCESS 0
#define RET_FAILURE -1
#endif

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

static struct itimerval *timer;
static sigset_t *blocker;
static bool enabled;

void alarm_yield(int signum)
{
	(void)signum;
	if(enabled)
		uthread_yield();
}

void preempt_disable(void)
{
	/* TODO Phase 4 */
	if(enabled)
		pthread_sigmask(SIG_BLOCK, blocker, NULL);
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
	if(enabled)
		pthread_sigmask(SIG_UNBLOCK, blocker, NULL);
}

void preempt_start(bool preempt)
{
	if(preempt) {
		/* we want a virtual timer */
		timer = malloc(sizeof(struct itimerval));
		enabled = true;
		if(signal(SIGALRM, alarm_yield) == SIG_ERR)
			fprintf(stderr, "Signal Error\n");
		/* setup the blocker */
		sigemptyset(blocker);
		sigaddset(blocker, SIGALRM);
		/* setup the timer */
		timer->it_interval.tv_sec = 0;
		timer->it_interval.tv_usec = 100*HZ;
		timer->it_value.tv_usec = 100*HZ;
		timer->it_value.tv_sec = 0;
		if(setitimer(ITIMER_REAL, timer, NULL) == RET_FAILURE)
			fprintf(stderr, "Timer Error\n");
	}
}

void preempt_stop(void)
{
	/* TODO Phase 4 */
	if(enabled) {
		timer->it_value.tv_usec = 0; //disables the timer
		free(timer);
		if(signal(SIGVTALRM, SIG_DFL) == SIG_ERR)
			fprintf(stderr, "DFL Signal Error\n");
	}
}

