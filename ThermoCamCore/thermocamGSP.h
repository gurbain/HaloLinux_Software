/*! 
* 	\file    ThermocamGSP.h
* 	\author  Gabriel Urbain <gurbain@mit.edu> - Visitor student at MIT SSL
* 	\date    July 2014
* 	\version 0.1
* 	\brief   Headers for Thermographic camera Guest Scientist Program on HaloCore
*
* 	License: The MIT License (MIT)
* 	Copyright (c) 2014, Massachussets Institute of Technology
*/

#ifndef THERMOCAMGSP_HH
#define THERMOCAMGSP_HH

class thermocamGSP;

// include all class headers here
#include "thermocam.h"
#include "utils.h"
#include "../HaloCore/GSP/networking.h"
#include "../HaloCore/GSP/videoStreaming.h"
#include "../HaloCore/GSP/haloGSP.h"

// OpenCV
#include "opencv2/core/core.hpp"

// C++ standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <ctime>
#include <cerrno>
#include <deque>


using namespace std;

class thermocamGSP:virtual public haloGSP
{ 

public:

	// Thermocam parameters
	string save_dir;
	
	// Thermocam  and images
	cv::Mat irImg;
	Thermocam thermo;

	// Constructor
	thermocamGSP(haloGSP *halo);
	
	// Functions
	void shutdown();
	void capture(cv::Mat& irImg);
	void captureAndSave();
	int init();
	void parseParameterFile(string line, bool printParams);

};

#endif