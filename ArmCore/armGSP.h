/*!
 *  \brief     VERTIGO Guest Scientist Program.
 *  \details   This is the primary GSP class for VERTIGO.
 *  \author    Konrad Makowka
 *  \author    Brent Tweddle
 *  \version   4.1a
 *  \date      2011-2012
 *  \copyright TBD
 */

#ifndef _DOCKINGPORTGSP_H_
#define _DOCKINGPORTGSP_H_

class dockingportGSP;

// include all class headers here
#include "dockingport_Camera.h"
#include "dockingport_DataStorage.h"
#include "dockingport_Rectifier.h"
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

//#define TEST_PROJ_CHANNEL_FILE "/opt/GogglesDaemon/TEST_PROJ_CHANNEL"
#define CAMERA_FILE "/opt/GogglesOptics/CAMERA_FILE"

class timestampedsingleImage {
	cv::Mat storeimg, singlestoreimg;
	double timestamp;
public:
	timestampedsingleImage(double _timestamp, cv::Mat& singleImage) {
			timestamp = _timestamp;
			storeimg.create(cv::Size(640,480), CV_8UC3);
			singlestoreimg = storeimg(cv::Rect(0,0,640,480));
			singleImage.copyTo(singlestoreimg);
	}

	void writeImg(dockingport_DataStorage& ds) {
		ds.timetagStorageFile << ds.numsaved << "," << timestamp << "\n";
		sprintf(ds.singleImageStorageName, "%s/SingleImage%d.bmp", ds.imagestoragepath, ds.numsaved);
		cv::imwrite(ds.singleImageStorageName, storeimg);
		ds.numsaved++;
	}

	void deallocImage() {
		storeimg.deallocate();
	}

	double getTimestamp() {
		return timestamp;
	}

};


class dockingportGSP : virtual public HaloGSP{


protected:
	// for timestamps
	double imageTimeStamp;
	double prevImageTimeStamp;

private:
//	double zerotime;
//	timeval timeRAW;

	struct timespec testStartTime, currentTime;

	// Mutexes
	pthread_mutex_t storageDeque_mutex;

	// Image Storage Thread
    void imageStorage_thread();



public:

//Moved to public from private
	pthread_t ImageStorageThread;
	std::deque<timestampedsingleImage> storageImg_deque;
//end moved to public from private

	//test time in millisecond
	double testtime;

	bool newImagesAvailable;
	bool stopImageStorageThread;
	int synchCheckFlag;
	
	char calibParamSetName[200]; // name of camera calibration set to be used, read in from command line
	char calibParamDir[200];

	cv::Mat singleImage;//, ImageCopy;

	dockingport_Camera camera;
	dockingport_Rectifier rectifier;
	dockingport_DataStorage datastorage;	


	//variables for reading image from file
	string camInputImgDir;
	int camInputStartImg;
	int camInputFinalImg;
	int camInputImgCounter;


	dockingportGSP() {

//		gettimeofday(&test,NULL);
//		zerotime = timeRAW.tv_sec*1000+timeRAW.tv_usec/1000; // Time when program was started in ms; zerotime calculated again when main program loop is entered

		camInputStartImg = 0;
		camInputFinalImg = -1;
		camInputImgCounter = 0;

		// internal flag init settings
		newImagesAvailable = false;
		stopImageStorageThread = false;
		synchCheckFlag = 0; // 0 means, that frames in current timestep are in synch
							// -1 if out of synch
							// this flag is only changed if camera.useSynchCams==true, since notSynch camera are out of synch anyway

		sprintf(calibParamSetName, "defaultSet"); // standard camera calibration set

	}

virtual void GSprocessSingleImage(cv::Mat& singleImg); // Single image processing science algorithms

//Moved to public from private
void shutdown();
void captureAndRectifySingleImage(cv::Mat& singleImg);
void init();
void parseParameterFile(string line);

    static void * imageStorage_thread_Helper(void * This)
    {
    	((dockingportGSP *)This)->imageStorage_thread();
    	return NULL;
    }
// end moved to public from private

};


#endif
