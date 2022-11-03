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
static struct itimerval *oldt;
static struct sigaction *sa;
static struct sigaction *oa;
static sigset_t *blocker;
static bool enabled;

void alarm_yield(int signum)
{
	(void)signum;
	if (enabled)
		uthread_yield();
}

void preempt_disable(void)
{
	if (enabled) {
		pthread_sigmask(SIG_BLOCK, blocker, NULL);
	}
}

void preempt_enable(void)
{
	if (enabled) {
		pthread_sigmask(SIG_UNBLOCK, blocker, NULL);
	}
}

void preempt_start(bool preempt)
{
	enabled = preempt;
	if (enabled) {
		/* Timer, sigaction, blocker init */
		timer = malloc(sizeof(struct itimerval));
		oldt = malloc(sizeof(struct itimerval));
		sa = malloc(sizeof(struct sigaction));
		oa = malloc(sizeof(struct sigaction));
		blocker = malloc(sizeof(sigset_t));
		
		timer->it_interval.tv_sec = 0;
		timer->it_interval.tv_usec = 100*HZ;
		timer->it_value.tv_sec = 0;
		timer->it_value.tv_usec = 100*HZ;
		sigemptyset(blocker);
		sigaddset(blocker, SIGVTALRM);
		sa->sa_handler = alarm_yield;
		sa->sa_mask = *blocker;

		if (sigaction(SIGVTALRM, sa, oa) == RET_FAILURE)
			fprintf(stderr, "Signal Error\n");
		if (getitimer(ITIMER_VIRTUAL, oldt) == RET_FAILURE)
			fprintf(stderr, "OLD Timer Error\n");
		if (setitimer(ITIMER_VIRTUAL, timer, NULL) == RET_FAILURE)
			fprintf(stderr, "Timer Error\n");
	}
}

void preempt_stop(void)
{
	if (enabled) {
		/* Disable the timer */
		timer->it_value.tv_usec = 0; 
		if (sigaction(SIGVTALRM, oa, NULL) == RET_FAILURE)
			fprintf(stderr, "DFL Signal Error\n");
		if (setitimer(ITIMER_VIRTUAL, oldt, NULL) == RET_FAILURE)
			fprintf(stderr, "DFL Timer Error\n");

		free(blocker);
		free(timer);
		free(oldt);
		free(sa);
		free(oa);
	}
}
