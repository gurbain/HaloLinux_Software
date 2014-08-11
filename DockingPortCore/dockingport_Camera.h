#ifndef __DP_OPTICS__

#define __DP_OPTICS__

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

typedef struct _DP_UEYE_IMAGE
{
    char    *pBuf;
    int     img_id;
    int     img_seqNum;
    int     nBufferSize;
} DP_UEYE_IMAGE;

class dockingport_Camera
{
	int iterDummy;

	HIDS h_cam1; 
	CAMINFO camInfo1; 
	char * act_img_buf1;
	char * last_img_buf1;
	DP_UEYE_IMAGE Cam1_ringbuffer[RING_BUFFER_SIZE];
	char CAMERA_1_SERIAL[12]; //HL changed to 12 from 11

	// dummy buffers for checking camera synch
	char *bufDummy1;

	// for image timestamp
	double currenttime, zerotime;
	timeval timeRAW;

	// private camera parameters, that are be accesible by get/set functions
	float frameRate;
	int exposureTime;

	// private camera parameters, that are be accesible by get functions
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

public:
	// will be written to from parameterFile.txt
	// contain serial numbers of both goggles involved in test
	// left serial number of camera has to be one of those two serial numbers

	char CAMERA_1_SERIAL_temp1[11];
	char CAMERA_1_SERIAL_temp2[11];

	//used in DP parseUDPSerialFile
	char SINGLE_CAM_SERIAL[100][11];
	int OPTICS_ID[100];

	// image number buffers for ring buffering --> MAKE THIS PRIVATE
	int singleImgNum;

	bool reduceImageSizeTo320x240;
	bool useCamera; // for ground testing without camera

	dockingport_Camera() // with default settings // HL
	{
		iterDummy = 0;
		// Flags
		reduceImageSizeTo320x240 = true;
		useCamera = true; // IMPORTANT: this is the flight setting, camera in use

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

	// these get and set functions return the actual vaules set in the camera hardware and are therefore of type double
	double getFrameRate();
	void setFrameRate(float newFR);
	double getExposureTime();
	void setExposureTime(int newET);
	void setHWGain(int gain);
	char* getCam1Serial();
	void setCam1Serial(const char* serialNum);
	void sethCam(HIDS hcam);

	unsigned int initOneCamera();
	unsigned int startOneCamera();
	unsigned int stopOneCamera();
	unsigned int closeOneCamera();
	unsigned int captureOneImage(cv::Mat& newImageFrame);
};

#endif
