/*! 
* 	\file    orfGSP.h
* 	\author  Gabriel Urbain <gurbain@mit.edu> - Visitor student at MIT SSL
* 	\date    July 2014
* 	\version 0.1
* 	\brief   Headers for ORF Guest Scientist Program on HaloCore
*
* 	License: The MIT License (MIT)
* 	Copyright (c) 2014, Massachussets Institute of Technology
*/

#ifndef ORFGSP_HH
#define ORFGSP_HH

class orfGSP;

// include all class headers here
#include "orf.h"
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

#define UDP_SERIAL_FILE "../DockingPortCore/UDP_SERIAL_FILE"

using namespace std;
/*
class timestampedsingleImage 
{
	private:
		cv::Mat storeimg, singlestoreimg;
		double timestamp;

	public:
		timestampedsingleImage(double _timestamp, cv::Mat& singleImage);
		void writeImg(dockingport_DataStorage& ds);
		void deallocImage();
		double getTimestamp();

};*/

class orfGSP:virtual public haloGSP
{ 

private:
	// Image Storage Thread
	void imageStorage_thread();
	static void * imageStorage_thread_Helper(void * This)
	{
		((orfGSP *)This)->imageStorage_thread();
		return NULL;
	};

public:

	bool newImagesAvailable;
	bool stopImageStorageThread;
	int synchCheckFlag;
	
	// ORF cam parameters
	bool auto_exposure;
	int integration_time;
	int modulation_freq;
	int amp_threshold;
	string ether_addr;
	string save_dir;
	string calib_filename;

	// ORF and images
	cv::Mat depth, visual, confidency;
	ORF orf;

	// Constructor
	orfGSP(haloGSP *halo);
	
	// Functions
	void shutdown();
	void captureAndRectify(cv::Mat& depthImg, cv::Mat& visualImg, cv::Mat& confidencyImg);
	void captureAndSaveRectified();
	int init();
	void parseParameterFile(string line, bool printParams);

};

#endif