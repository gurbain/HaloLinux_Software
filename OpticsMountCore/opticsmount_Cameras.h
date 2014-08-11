#ifndef __OPTICS__

#define __OPTICS__

#define __LINUX__

#include <stdio.h>
#include <string.h>
#include <iostream>

// uEye camera library
#include <uEye.h>
#include <ueye_deprecated.h>

// OpenCV
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

// C++ includes
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

#define RING_BUFFER_SIZE	10
#define WAIT_TIMEOUT_MS		10000

typedef struct _UEYE_IMAGE
{
    char    *pBuf;
    int     img_id;
    int     img_seqNum;
    int     nBufferSize;
} UEYE_IMAGE;

class opticsmount_Cameras
{
	int iterDummy;

	CAMINFO camInfo1;
	CAMINFO camInfo2;
	HIDS h_cam1;
	HIDS h_cam2;
	char * act_img_buf1;
	char * act_img_buf2;
	char * last_img_buf1;
	char * last_img_buf2;
	UEYE_IMAGE Cam1_ringbuffer[RING_BUFFER_SIZE];
	UEYE_IMAGE Cam2_ringbuffer[RING_BUFFER_SIZE];
	char CAMERA_1_SERIAL[11];

	// dummy buffers for checking camera synch
	char *bufDummy1, *bufDummy2;

	// for image timestamp
	double currenttime, zerotime;
	timeval timeRAW;

	// private cameras parameters, that are be accesible by get/set functions
	float frameRate;
	int exposureTime;

	// private cameras parameters, that are be accesible by get functions
	int imgWidth;
	int imgHeight;

	int hwGain;

	// private camera parameters, that the Guest Scientist does not need to have access to
	int pixelClockFreq; // in MHz
	float maxFrameRate;
	int flashDelay;
	int flashDuration;
	int imgBufferCount;
	int imgBitsPixel;
	int numberOfBytesToCopy;

	int prevImgNum;

	unsigned int initTwoCamerasNotSynch();
	unsigned int closeTwoCamerasNotSynch();
	unsigned int startTwoCamerasNotSynch();
	unsigned int stopTwoCamerasNotSynch();
	unsigned int captureTwoImagesNotSynch(cv::Mat& leftNewImageFrame, cv::Mat& rightNewImageFrame, int* img_num1, int* img_num2);

	unsigned int initTwoCamerasSynch();
	unsigned int closeTwoCamerasSynch();
	unsigned int startTwoCamerasSynch();
	unsigned int stopTwoCamerasSynch();
	unsigned int captureTwoImagesSynch(cv::Mat& leftNewImageFrame, cv::Mat& rightNewImageFrame, int* img_num1, int* img_num2, int& synchCheckFlag);

public:

	// will be written to from parameterFile.txt
	// contain serial numbers of both goggles involved in test
	// left serial number of camera has to be one of those two serial numbers
	char CAMERA_1_SERIAL_temp1[11];
	char CAMERA_1_SERIAL_temp2[11];

	char LEFT_CAM_SERIAL[16][11];
	int OPTICS_ID[16];

	// image number buffers for ring buffering --> MAKE THIS PRIVATE
	int leftImgNum;
	int rightImgNum;

	bool useSynchCams;
	bool reduceImageSizeTo320x240;
	bool useCameras; // for ground testing without cameras

	opticsmount_Cameras() // with default settings
	{
		iterDummy = 0;
		// Flags
		useSynchCams = true;
		reduceImageSizeTo320x240 = true;
		useCameras = true; // IMPORTANT: this is the flight setting, cameras in use

		sprintf(CAMERA_1_SERIAL, "4002795734"); // this is the serial number of the left camera, a standard initialization

		pixelClockFreq = 10; // in MHz
		frameRate = 5;
		exposureTime = 20;
		flashDelay = 0;
		flashDuration = 15000;

		// these values are fixed in the constructor an cannot be changed
		imgWidth = 640;
		imgHeight = 480;
		imgBitsPixel = 8;
		imgBufferCount = RING_BUFFER_SIZE;

		hwGain = 100;

	//	*leftImgNum = 0;
	//	*rightImgNum = 0;

		// read parameter file
		// if this file not available default settings will be used

	}

	int getImageWidth();
	int getImageHeight();

	// these get and set functions return the actual values set in the camera hardware and are therefore of type double
	double getFrameRate();
	void setFrameRate(float newFR);
	double getExposureTime();
	void setExposureTime(int newET);
	void setHWGain(int gain);
	char* getCam1Serial();
	void setCam1Serial(const char* serialNum);
	void sethCam(HIDS h_cam);

	unsigned int initTwoCameras();
	unsigned int closeTwoCameras();
	unsigned int startTwoCameras();
	unsigned int stopTwoCameras();
	unsigned int captureTwoImages(cv::Mat& leftNewImageFrame, cv::Mat& rightNewImageFrame, int* img_num1, int* img_num2, int& synchCheckFlag);

};

#endif
