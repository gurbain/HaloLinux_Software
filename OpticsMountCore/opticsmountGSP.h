/*!
 *  \brief     VERTIGO Guest Scientist Program.
 *  \details   This is the primary GSP class for VERTIGO.
 *  \author    Konrad Makowka
 *  \author    Brent Tweddle
 *  \version   4.1a
 *  \date      2011-2012
 *  \copyright TBD
 */

#ifndef _OPTICSMOUNTGSP_H_
#define _OPTICSMOUNTGSP_H_

class opticsmountGSP;

// include all class headers here
#include "opticsmount_Cameras.h"
#include "opticsmount_DataStorage.h"
#include "opticsmount_Rectifier.h"
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

class timestampedImage {
public: //HL TEMP 
	cv::Mat storeimg, leftstoreimg, rightstoreimg;
	double timestamp;

	timestampedImage(double _timestamp, cv::Mat& leftImage, cv::Mat& rightImage) {
		timestamp = _timestamp;
		storeimg.create(cv::Size(640*2,480), CV_8UC1);
		leftstoreimg = storeimg(cv::Rect(0,0,640,480));
		rightstoreimg = storeimg(cv::Rect(640,0,640,480));
		leftImage.copyTo(leftstoreimg);
		rightImage.copyTo(rightstoreimg);
	}

	void writeImg(opticsmount_DataStorage& ds) {
		ds.timetagStorageFile << ds.numsaved << "," << timestamp << "\n";
		sprintf(ds.leftRightImageStorageName, "%s/LeftRight%d.bmp", ds.imagestoragepath, ds.numsaved);
		cv::imwrite(ds.leftRightImageStorageName, storeimg);
		ds.numsaved++;
	}

	void deallocImage() {
		storeimg.deallocate();
	}

	double getTimestamp() {
		return timestamp;
	}

};

class opticsmountGSP : virtual public haloGSP{

protected:
	// for timestamps
	double imageTimeStamp;
	double prevImageTimeStamp;

private:

	struct timespec testStartTime, currentTime;

	// Mutexes
	pthread_mutex_t storageDeque_mutex;
	// Copied from below to create one Private section (DS)
	// Image Storage Thread
    	void imageStorage_thread();
	
public:

	pthread_t ImageStorageThread;
	std::deque<timestampedImage> storageImg_deque;

	//test time in millisecond
	double testtime;

	// GSP Flags
	bool newImagesAvailable;
	bool stopImageStorageThread;
	int synchCheckFlag;

	// Variables to store paths
	char calibParamSetName[200]; // name of camera calibration set to be used, read in from command line
	char calibParamDir[200];

	cv::Mat leftImage;//, leftImageCopy;
	cv::Mat rightImage;//, rightImageCopy;

	opticsmount_Cameras cameras;
	opticsmount_Rectifier rectifier;
	opticsmount_DataStorage datastorage;

	//variables for reading image from file
	string camInputImgDir;
	int camInputStartImg;
	int camInputFinalImg;
	int camInputImgCounter;

	HIDS camera_ID;
	char opticsmountName[200];

	//CONSTRUCTOR FOR OPTICSMOUNT
	opticsmountGSP(haloGSP *halo,  HIDS _camera_ID) {

		//Set camera hcam values
		cameras.sethCam(_camera_ID); //sets the left camera hcam value, OM only needs to set left camera value to distinguish cameras
		camera_ID = _camera_ID; //use hcam of cameras for datastorage

		//Set path names to HaloGSP's path names
		sprintf(this->GPFpath, "%s", halo->GPFpath);
		sprintf(this->calibParamSetName, "%s", halo->calibParamSetName);
		sprintf(this->parameterFilePath, "%s", halo->parameterFilePath);
		sprintf(this->testname, "%s", halo->testname);
		sprintf(this->opticsmountName, "OPTICSMOUNT");
		sprintf(this->runPath, "%s", halo->runPath);

		this->cameras.reduceImageSizeTo320x240=false;
        camInputStartImg = 0;
        camInputFinalImg = -1;
        camInputImgCounter = 0;

		this->parseOMCameraFile(); //parses CAMERA_FILE
        
       	// internal flag init settings
        newImagesAvailable = false;
        stopImageStorageThread = false;
        synchCheckFlag = 0; // 0 means, that frames in current timestep are in synch
                            // -1 if out of synch
                            // this flag is only changed if cameras.useSynchCams==true, since notSynch cameras are out of synch anyway
        if (calibParamSetName[0] == '\0') { //HL added, check if string is empty, i.e. the first character is the null terminating character
        		sprintf(calibParamSetName, "defaultSet"); // standard camera calibration set
        };
	}

    void captureAndRectifyImages(cv::Mat& leftImg, cv::Mat& rightImg);
	void init();
	void shutdown();
	void parseParameterFile(string line, bool printParams);
	void parseOMCameraFile(); //read the CAMERA_FILE
	static void * imageStorage_thread_Helper(void * This)
	{
		((opticsmountGSP *)This)->imageStorage_thread();
		return NULL;
    }

};

#endif
