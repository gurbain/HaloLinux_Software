/*
 * StopWatch.h
 *
 *  Created on: Feb 28, 2012
 *      Author: muggler
 */

#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <sys/time.h>

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <limits>
#include <cmath>

class StopWatch {
public:
	StopWatch();
	virtual ~StopWatch();

	void start(std::string label);
	void stop(std::string label);

	void stats(std::ostream &out) const;

private:
	std::map< std::string, int> labels;
	std::map< std::string, int>::iterator labelsIterator;

	std::vector< double > timestamps;
	std::vector< std::vector<double> > measurements;

	std::vector<std::string> orderedLabels;

	timeval timeRAW;
};

#endif /* STOPWATCH_H_ */
