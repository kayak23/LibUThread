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
	/* TODO Phase 4 */
	if (enabled) {
		pthread_sigmask(SIG_BLOCK, blocker, NULL);
		//fprintf(stderr, "[PRE] Preemption disabled.\n");
	}
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
	if (enabled) {
		pthread_sigmask(SIG_UNBLOCK, blocker, NULL);
		//fprintf(stderr, "[PRE] Premption enabled.\n");
	}
}

void preempt_start(bool preempt)
{
	enabled = preempt; //TODO: implement saving the old timer
	if (enabled) {
		/* timer, sigaction, blocker init */
		timer = malloc(sizeof(struct itimerval));
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
		if (setitimer(ITIMER_VIRTUAL, timer, NULL) == RET_FAILURE)
			fprintf(stderr, "Timer Error\n");
	}
}

void preempt_stop(void)
{
	/* TODO Phase 4 */
	if (enabled) {
		timer->it_value.tv_usec = 0; //disables the timer
		free(timer);
		free(sa);
		free(blocker);
		if (sigaction(SIGVTALRM, oa, NULL) == RET_FAILURE)
			fprintf(stderr, "DFL Signal Error\n");
		free(oa);
	}
}

