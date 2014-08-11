/*! 
* 	\file    utils.h
* 	\author  Gabriel Urbain <gurbain@mit.edu> - Visitor student at MIT SSL
* 	\date    August 2014
* 	\version 0.1
* 	\brief   Software global tools headers
*
* 	License: The MIT License (MIT)
* 	Copyright (c) 2014, Massachussets Institute of Technology
*/

#ifndef UTILS_HH
#define UTILS_HH

// Output streamings defines
#define __AUTHOR__	"Gabriel Urbain"
#define __F__		(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define ERROR		cout << "\033[31m["<<setprecision(3)<<((float)clock())/CLOCKS_PER_SEC<<"] "<<"ERROR in "<<__F__<<":"<<__LINE__<<": \033[00m"
#define DEBUG		cout << "\033[33m["<<setprecision(3)<<((float)clock())/CLOCKS_PER_SEC<<"] "<<"DEBUG in "<<__F__<<":"<<__LINE__<<": \033[00m"
#define INFO		cout << "["<<setprecision(3)<<((float)clock())/CLOCKS_PER_SEC<<"] "<<"INFO: "  

// OpenCV libs
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

// Common libs
#include <sys/utsname.h>
#include <stdint.h>
#include <iomanip>
#include <limits>
#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/version.h>
#include <sys/time.h>
	
// Defines
#define DELTATMIN	150
#define DELTATMAX	300

using namespace std;

static timeval timeValInit;

void init();

class TimeStamp {
	
	public:
		timeval timeBegin;
		timeval timeEnd;
		timeval timeInit;
		int meanTime;
		int procTime;
		int startTime;
		int stopTime;
		int initTime;
		bool isRunning;
	
	
		TimeStamp();
		~TimeStamp();
		
		void start();
		void stop();
		int getProcTime();
		int getMeanTime();
		int getStartTime();
		int getStopTime();
		bool isSynchro(TimeStamp t);
};


#endif
