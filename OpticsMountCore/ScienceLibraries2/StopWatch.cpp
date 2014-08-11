/*
 * StopWatch.cpp
 *
 *  Created on: Feb 28, 2012
 *      Author: muggler
 */

#include "StopWatch.h"

StopWatch::StopWatch() {
}

StopWatch::~StopWatch() {
}

void StopWatch::start(std::string label) {

	labelsIterator = labels.find(label);

	// Create if it does not exist
	if (labelsIterator == labels.end()) {
		labels.insert( std::pair<std::string, int> (label, labels.size()));
		timestamps.push_back(0);
		orderedLabels.push_back(label);

		std::vector<double> dummy;
		measurements.push_back(dummy);

		labelsIterator = labels.find(label);
	}

	gettimeofday(&timeRAW, NULL);
	timestamps[labelsIterator->second] = timeRAW.tv_sec*1000 + timeRAW.tv_usec/1000; //ms
}

void StopWatch::stop(std::string label) {
	gettimeofday(&timeRAW, NULL);
	double timestamp = timeRAW.tv_sec*1000 + timeRAW.tv_usec/1000; //ms

	labelsIterator = labels.find(label);

	if (labelsIterator != labels.end()) {
		double diff = timestamp - timestamps[labelsIterator->second];
		measurements[labelsIterator->second].push_back(diff);
	}
}

void StopWatch::stats(std::ostream &out) const {
	out << std::endl << "Stop Watch Statistics" << std::endl;
	out << std::setw (30) << "Name"
			<< std::setw (8) << "# Runs"
			<< std::setw (12) << "Avg [ms]"
			<< std::setw (12) << "Min [ms]"
			<< std::setw (12) << "Max [ms]"
			<< std::setw (12) << "Total [ms]"
			<< std::endl;

	std::map< std::string, int>::const_iterator it;
	for (int i=0; i < (int)orderedLabels.size(); i++) {
		it = labels.find(orderedLabels[i]);
		double sum=0, max=0.;
		double min=std::numeric_limits<double>::max();
		int num=measurements[it->second].size();

		for (int i=0; i<num; i++) {
			double meas = measurements[it->second][i];
			sum += meas;
			if (meas < min) min = meas;
			if (meas > max) max = meas;
		}
		double mean = round(sum/num);

		out << std::setw (30) << it->first;
		out << std::setw (8) << num;
		out << std::setw (12) << mean;
		out << std::setw (12) << min;
		out << std::setw (12) << max;
		out << std::setw (12) << sum;

		out << std::endl;
	}
}
