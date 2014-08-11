#include "gogglesTiming.h"

double gogglesTiming::timeDiff(struct timespec *start, struct timespec *end)
{
	double milliseconds;
	double tv_sec, tv_nsec;
	if ((end->tv_nsec-start->tv_nsec)<0) {
		tv_sec = end->tv_sec-start->tv_sec-1;
		tv_nsec = 1.0e9+end->tv_nsec-start->tv_nsec;
	} else {
		tv_sec = end->tv_sec-start->tv_sec;
		tv_nsec = end->tv_nsec-start->tv_nsec;
	}
	milliseconds = tv_sec*1.0e3 + (tv_nsec / 1.0e6);
	return milliseconds;
}
