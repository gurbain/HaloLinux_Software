/*
 * gogglesTiming.h
 *
 *  Created on: Oct 17, 2013
 *      Author: tweddle
 */

#ifndef GOGGLESTIMING_H_
#define GOGGLESTIMING_H_

#include <sys/time.h>


namespace gogglesTiming //optional
{

	double timeDiff(struct timespec *start, struct timespec *end);
}


#endif /* GOGGLESTIMING_H_ */
