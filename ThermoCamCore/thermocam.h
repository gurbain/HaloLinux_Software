/*! 
* 	\file    thermocam.h
* 	\author  Gabriel Urbain <gurbain@mit.edu> - Visitor student at MIT SSL
* 	\date    July 2014
* 	\version 0.1
* 	\brief   Headers for ethernet thermographic camera
*
* 	License: The MIT License (MIT)
* 	Copyright (c) 2014, Massachussets Institute of Technology
*/

#ifndef THERMOCAM_HH
#define THERMOCAM_HH

// OpenCV libs
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

// Thermocam libs
#include <PvSampleUtils.h>
#include <PvDevice.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvInterface.h>
#include <PvSystem.h>
#include <PvBufferWriter.h>
#include <PvPixelType.h>
#include <PvString.h>
#include <PvBufferWriter.h>
#include <PvBufferConverter.h>
#include <PvSystem.h>
#include <PvPipeline.h>

// Common libs
#include <stdint.h>
#include <iomanip>
#include <limits>
#include <stdio.h>
#include <iostream>
#include <string>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <signal.h>
#include <sys/stat.h>
#include <cstdio>
#include <vector>
#include <ctime>

// Project libs
#include "utils.h"

// Thermocam defines
#define BUFFER_COUNT (16)

using namespace std;
using namespace cv;

class Thermocam {

	public:
		// Public functions
		Thermocam ();
		~Thermocam ();

		int init(string img_directory="img");
		void close();
		int capture(Mat& img, TimeStamp& ts);
		int captureAndSave();

	private:
		// Camera variables
		PvSystem lSystem;
		PvDevice lDevice;
		PvStream lStream;
		PvPipeline *lPipeline;
		PvImage *lImage;
		
		PvDeviceInfo *lDeviceInfo;
		PvGenParameterArray *lDeviceParams;
		PvGenParameterArray *lStreamParams;

		PvResult lResult;
		PvUInt32 lInterfaceCount;
		
		PvGenInteger *lPayloadSize;
		PvGenCommand *lStart;
		PvGenCommand *lStop;
		PvGenCommand *lTimeStamp;

		PvInt64 lCountVal;
		double lFrameRateVal;
		double lBandwidthVal;

		PvInt64 lSize;
		PvInt64 lWidth, lHeight;
		

		// File save parameters
		ofstream tsfile;
		string timestamps;
		int imgNum;
		int tslast;
		string img_directory;

		// Private functions
		int listCam();
		int connect();
		void startPipeline();
};

#endif
