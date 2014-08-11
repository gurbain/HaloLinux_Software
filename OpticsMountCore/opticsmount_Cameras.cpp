#include "opticsmount_Cameras.h"

int opticsmount_Cameras::getImageWidth()
{
	return this->imgWidth;
}

int opticsmount_Cameras::getImageHeight()
{
	return this->imgHeight;
}

double opticsmount_Cameras::getFrameRate()
{
	if (this->useCameras) {
		int retVal;
		double fr;

		retVal = is_SetFrameRate(h_cam1, IS_GET_FRAMERATE, &fr);
		if (retVal != IS_SUCCESS)
		{
			printf("Could not read Frame Rate from Camera (Code: %d)\n", retVal);
			return retVal;
		}

		return fr;
	} 
	else {
		return this->frameRate;
	}
}

void opticsmount_Cameras::setFrameRate(float newFR)
{
	this->frameRate = newFR;
}

double opticsmount_Cameras::getExposureTime()
{
	int retVal;
	double exp_time;

	retVal = is_SetExposureTime(h_cam1, IS_GET_EXPOSURE_TIME, &exp_time);
	if (retVal != IS_SUCCESS)
	{
		printf("Could not read Exposure Time from Camera (Code: %d)\n", retVal);
		return retVal;
	}

	return exp_time;
}

void opticsmount_Cameras::setExposureTime(int newET)
{
	this->exposureTime = newET;
}

void opticsmount_Cameras::setHWGain(int gain)
{
	this->hwGain = gain;
}

char* opticsmount_Cameras::getCam1Serial()
{
	return this->CAMERA_1_SERIAL;
}

void opticsmount_Cameras::setCam1Serial(const char* serialNum)
{
	sprintf(this->CAMERA_1_SERIAL, "%s", serialNum);
}

void opticsmount_Cameras::sethCam(HIDS h_cam)
{
	this->h_cam1 = h_cam;
	this->h_cam2 = h_cam + 1; //IMPORTANT: change right camera h_cam to your input h_cam (set in TP) + 1 to be able to connect properly
}

unsigned int opticsmount_Cameras::initTwoCameras()
{
	if (this->reduceImageSizeTo320x240)
	{
		this->numberOfBytesToCopy = 76800;
	}
	else
	{
		this->numberOfBytesToCopy = 307200;
	}

	if (this->useSynchCams)
	{
		this->initTwoCamerasSynch();
	}

	else
	{
		this->initTwoCamerasNotSynch();
	}
}

unsigned int opticsmount_Cameras::closeTwoCameras()
{
	if (this->useSynchCams)
	{
		this->closeTwoCamerasSynch();
	}

	else
	{
		this->closeTwoCamerasNotSynch();
	}
}
unsigned int opticsmount_Cameras::startTwoCameras()
{
	if (this->useSynchCams)
	{
		this->startTwoCamerasSynch();
	}

	else
	{
		this->startTwoCamerasNotSynch();
	}
}

unsigned int opticsmount_Cameras::stopTwoCameras()
{
	if (this->useSynchCams)
	{
		this->stopTwoCamerasSynch();
	}

	else
	{
		this->stopTwoCamerasNotSynch();
	}
}

unsigned int opticsmount_Cameras::captureTwoImages(cv::Mat& leftNewImageFrame, cv::Mat& rightNewImageFrame, int* img_num1, int* img_num2, int& synchCheckFlag)
{
	
	cout << "captureTwoImages entered" << endl;

	if (this->useSynchCams)
	{
		this->captureTwoImagesSynch(leftNewImageFrame, rightNewImageFrame, img_num1, img_num2, synchCheckFlag);
	}

	else
	{
		this->captureTwoImagesNotSynch(leftNewImageFrame, rightNewImageFrame, img_num1, img_num2);
	}
}

///////////////////////////////////////////////////////////////
//// Begin of Synch functions
unsigned int opticsmount_Cameras::initTwoCamerasSynch()
{
	int img_xpos, img_ypos, img_width, img_height;
	int bits_pixel;
	double fps, exp_time;
	int retVal;
	int i;

	memset(&camInfo1,0,sizeof(CAMINFO));
	memset(&camInfo2,0,sizeof(CAMINFO));

	this->maxFrameRate = float(this->pixelClockFreq * 1000000) / float((752*480));
	if (this->frameRate > this->maxFrameRate)
	{
		printf("Specified FrameRate is higher than camera pixel clock frequency allows! --- frameRate will be set to maximally possible value of %f\n", this->maxFrameRate);
		this->frameRate = this->maxFrameRate;
	}

	//initialize camera 1 & 2
	retVal = is_InitCamera(&h_cam1,0);
	if (retVal != IS_SUCCESS)
	{
		h_cam1 = 0;
		printf("Init Camera Failed (Code: %d)\n",retVal);
		return retVal;
	}

	retVal = is_InitCamera(&h_cam2,0);
	if (retVal != IS_SUCCESS)
	{
		h_cam2 = 0;
		printf("Init Camera Failed (Code: %d)\n",retVal);
		return retVal;
	}

	//Get camera info 1 & 2
	retVal = is_GetCameraInfo(h_cam1, &camInfo1);
	if (retVal != IS_SUCCESS)
	{
		printf("Get Camera Info Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("S/N of CAM 1: %s - Select: %d\n", camInfo1.SerNo, camInfo1.Select);
	retVal = is_GetCameraInfo(h_cam2, &camInfo2);
	if (retVal != IS_SUCCESS)
	{
		printf("Get Camera Info Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("S/N of CAM 2: %s - Select: %d\n", camInfo2.SerNo, camInfo2.Select);

	for (int count = 0; count < 16; count++) {
		if (OPTICS_ID[count] == -1) {
			cout << "CAMERA S/N Not Found" << endl;
			break;
		}
		if (strcmp(camInfo1.SerNo,LEFT_CAM_SERIAL[count]) == 0) {
			strcpy(this->CAMERA_1_SERIAL, camInfo1.SerNo);
			cout << "LEFT CAM S/N: " << this->CAMERA_1_SERIAL << " (Not Swapped)" << endl;
			cout << "OPTICS MOUNT S/N: " << OPTICS_ID[count] << endl;
			break;
		} else if (strcmp(camInfo2.SerNo,LEFT_CAM_SERIAL[count]) == 0) {
			strcpy(this->CAMERA_1_SERIAL, camInfo2.SerNo);
			cout << "LEFT CAM S/N: " << this->CAMERA_1_SERIAL << " (Swapped)" << endl;
			cout << "OPTICS MOUNT S/N: " << OPTICS_ID[count] << endl;
			HIDS h_cam_temp;
			h_cam_temp = h_cam2;
			h_cam2 = h_cam1;
			h_cam1 = h_cam_temp;
			break;
		}
	}

	// set pixelClock
	retVal = is_SetPixelClock (h_cam1, this->pixelClockFreq);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Pixel Clock Failed (Code: %d)\n", retVal);
		return retVal;
	}

	retVal = is_SetPixelClock (h_cam2, this->pixelClockFreq);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Pixel Clock Failed (Code: %d)\n", retVal);
		return retVal;
	}

	//get color modes 1 & 2
	bits_pixel = this->imgBitsPixel;
	retVal = is_SetColorMode(h_cam1, IS_SET_CM_Y8);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Color Mode Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_SetColorMode(h_cam2, IS_SET_CM_Y8);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Color Mode Failed (Code: %d)\n", retVal);
		return retVal;
	}

	if (reduceImageSizeTo320x240)
	{
		retVal = is_SetBinning(h_cam1, IS_BINNING_2X_VERTICAL || IS_BINNING_2X_HORIZONTAL);
		if (retVal != IS_SUCCESS)
		{
			printf("Set Binning failed for Camera 1 (Code: %d)\n", retVal);
			return retVal;
		}

		retVal = is_SetBinning(h_cam2, IS_BINNING_2X_VERTICAL || IS_BINNING_2X_HORIZONTAL);
		if (retVal != IS_SUCCESS)
		{
			printf("Set Binning failed for Camera 2 (Code: %d)\n", retVal);
			return retVal;
		}

		this->imgHeight = int( (this->imgHeight)/2 );
		this->imgWidth = int( (this->imgWidth)/2 );
	}

	//set area of interest 1 & 2
	img_width = this->imgWidth;
	img_height = this->imgHeight;

	if (reduceImageSizeTo320x240)
	{
		img_ypos = 0;
		img_xpos = 28;	//shifts the area of interest so that the image in the center of the ccd
	}
	else
	{
		img_ypos = 0;
		img_xpos = 56;	//shifts the area of interest so that the image in the center of the ccd
	}

	retVal = is_SetAOI(h_cam1, IS_SET_IMAGE_AOI, &img_xpos, &img_ypos, &img_width, &img_height);
	if (retVal != IS_SUCCESS)
	{
		printf("SetAOI Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_SetAOI(h_cam2, IS_SET_IMAGE_AOI, &img_xpos, &img_ypos, &img_width, &img_height);
	if (retVal != IS_SUCCESS)
	{
		printf("SetAOI Failed (Code: %d)\n", retVal);
		return retVal;
	}

	for(i = 0; i < this->imgBufferCount; i++)
	{
		//cam 1
		//allocate image memory
		retVal = is_AllocImageMem(h_cam1, img_width, img_height, bits_pixel, &(this->Cam1_ringbuffer[i].pBuf), &(this->Cam1_ringbuffer[i].img_id) );
		if (retVal != IS_SUCCESS)
		{
			printf("AllocImageMem Failed (Code: %d)\n", retVal);
			return retVal;
		}

		//add to sequence
		retVal = is_AddToSequence(h_cam1, this->Cam1_ringbuffer[i].pBuf, this->Cam1_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("AddToSequence Failed (Code: %d)\n", retVal);
			return retVal;
		}
		//printf("Image Buffer Address(%d): 0x%08X\n",i, img_buffer1[i].pBuf);

		//cam 2
		//allocate image memory
		retVal = is_AllocImageMem(h_cam2, img_width, img_height, bits_pixel, &this->Cam2_ringbuffer[i].pBuf, &this->Cam2_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("AllocImageMem Failed (Code: %d)\n", retVal);
			return retVal;
		}

		//add to sequence
		retVal = is_AddToSequence(h_cam2, this->Cam2_ringbuffer[i].pBuf, this->Cam2_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("AddToSequence Failed (Code: %d)\n", retVal);
			return retVal;
		}
		//printf("Image Buffer Address(%d): 0x%08X\n",i, img_buffer2[i].pBuf);
	}

	//set up frame rate 1 & 2
	retVal = is_SetFrameRate(h_cam1, this->frameRate, &fps);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Frame Rate Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("New Frame rate: %f\n",fps);
	retVal = is_SetFrameRate(h_cam2, this->frameRate, &fps);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Frame Rate Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("New Frame rate: %f\n",fps);

	//set up exposure time 1 & 2
	retVal = is_SetExposureTime(h_cam1, this->exposureTime, &exp_time);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Exposure Time Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("New Exposure Time: %f\n",exp_time);
	retVal = is_SetExposureTime(h_cam2, this->exposureTime, &exp_time);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Exposure Time Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("New Exposure Time: %f\n",exp_time);

	//set gain value
	retVal = is_SetGainBoost(h_cam1, IS_SET_GAINBOOST_OFF);
	if (retVal != IS_SUCCESS)
	{
		printf("Enable GainBoost Failed (Code: %d)\n", retVal);
		return retVal;
	}

	retVal = is_SetGainBoost(h_cam2, IS_SET_GAINBOOST_OFF);
	if (retVal != IS_SUCCESS)
	{
		printf("Enable GainBoost Failed (Code: %d)\n", retVal);
		return retVal;
	}

	retVal = is_SetHardwareGain(h_cam1, this->hwGain, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
	if (retVal != IS_SUCCESS)
	{
		printf("Set HW Gain Failed (Code: %d)\n", retVal);
		return retVal;
	}
//	retVal = is_SetHardwareGain(h_cam1, IS_GET_MASTER_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
//	printf("New Gain Factor: %d\n", retVal);

	retVal = is_SetHardwareGain(h_cam2, this->hwGain, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
	if (retVal != IS_SUCCESS)
	{
		printf("Set HW Gain Failed (Code: %d)\n", retVal);
		return retVal;
	}
//	retVal = is_SetHardwareGain(h_cam2, IS_GET_MASTER_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
//	printf("New Gain Factor: %d\n", retVal);


	//set camera 1 to wait for a trigger on the digital input line
	//from camera 2 (falling edge is all that is available for the hw)
	retVal = is_SetExternalTrigger (h_cam1, IS_SET_TRIGGER_HI_LO);
	if (retVal != IS_SUCCESS)
	{
		printf("Set External Trigger Failed (Code: %d)\n", retVal);
		return retVal;
	}

	//set up flash from camera 2 to have a falling edge when there is an exposure
	//this triggers camera 1
	retVal = is_SetFlashStrobe(h_cam2, IS_SET_FLASH_LO_ACTIVE_FREERUN, 0);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Flash Strobe Failed (Code: %d)\n", retVal);
		return retVal;
	}

	//set up flash delay and duration on camera 2
	//0 us delay and 5 ms duration of the low pulse
	// Cam2 drives Cam1 using the flashStrobe, but the current flash is for the next image capture
	int flashDelayTemp =  int( 1000000 / (this->frameRate) - (this->exposureTime)*1000 );
	retVal = is_SetFlashDelay(h_cam2, flashDelayTemp, 5000);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Flash Delay Failed (Code: %d)\n", retVal);
		return retVal;
	}

	//set up the flash on camera 1 to drive the led lights
	//high whenever
	retVal = is_SetFlashStrobe(h_cam1, IS_SET_FLASH_HI_ACTIVE, 0);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Flash Strobe Failed (Code: %d)\n", retVal);
		return retVal;
	}

	//set up LED delay/duration to be customizable in optics2.h
	retVal = is_SetFlashDelay(h_cam1, this->flashDelay, this->flashDuration);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Flash Delay Failed (Code: %d)\n", retVal);
		return retVal;
	}

	//Enable frame event
	retVal = is_EnableEvent(h_cam1, IS_SET_EVENT_FRAME);        // enable frame event
	if (retVal != IS_SUCCESS)
	{
		printf("Enable Event Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_EnableEvent(h_cam2, IS_SET_EVENT_FRAME);        // enable frame event
	if (retVal != IS_SUCCESS)
	{
		printf("Enable Event Failed (Code: %d)\n", retVal);
		return retVal;
	}

	return 0;
}

unsigned int opticsmount_Cameras::closeTwoCamerasSynch()
{
	int i, retVal;

	for(i = 0; i < this->imgBufferCount; i++)
	{
		//deallocate image memory
		retVal = is_FreeImageMem(h_cam1, this->Cam1_ringbuffer[i].pBuf, this->Cam1_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("FreeImageMem Failed (Code: %d)\n", retVal);
			return retVal;
		}
		this->Cam1_ringbuffer[i].pBuf = NULL;
		this->Cam1_ringbuffer[i].img_id = 0;

		retVal = is_FreeImageMem(h_cam2, this->Cam2_ringbuffer[i].pBuf, this->Cam2_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("FreeImageMem Failed (Code: %d)\n", retVal);
			return retVal;
		}
		this->Cam2_ringbuffer[i].pBuf = NULL;
		this->Cam2_ringbuffer[i].img_id = 0;

	}

	//close camera 1 & 2
	if(h_cam1)
	{
		is_ExitCamera(h_cam1);
	}
	h_cam1 = 0;
	if(h_cam2)
	{
		is_ExitCamera(h_cam2);
	}
	h_cam2 = 0;

	return 0;
}

unsigned int opticsmount_Cameras::startTwoCamerasSynch()
{
	int retVal;

	//start capture 1 & 2

	retVal = is_CaptureVideo(h_cam2, IS_DONT_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Capture Video Failed (Code: %d)\n", retVal);
		return retVal;
	}

	retVal = is_CaptureVideo(h_cam1, IS_DONT_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Capture Video Failed (Code: %d)\n", retVal);
		return retVal;
	}

	return 0;
}

unsigned int opticsmount_Cameras::stopTwoCamerasSynch()
{
	int retVal;

	retVal = is_StopLiveVideo(h_cam1, IS_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Stop Live Video Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_StopLiveVideo(h_cam2, IS_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Stop Live Video Failed (Code: %d)\n", retVal);
		return retVal;
	}

	return 0;
}


unsigned int opticsmount_Cameras::captureTwoImagesSynch(cv::Mat& leftNewImageFrame, cv::Mat& rightNewImageFrame, int* img_num1, int* img_num2, int& synchCheckFlag)
{
	int retVal;

	iterDummy++;

	//this function is not documented in the programmers manual
	//it is mentioned in README.TXT and used in the demo program...
	//the parameters are (camera_handle, event_type, timeout_in_ms)

	/*
	retVal = is_WaitEvent(h_cam1, IS_SET_EVENT_FRAME, WAIT_TIMEOUT_MS);
	if (retVal != IS_SUCCESS)
	{
		printf("Is wait event Failed (Code: %d)\n", retVal);
		return retVal;
	}
	*/
	if (iterDummy > 1)
	{

		// Unlock active image buffer for camera 2
		retVal = is_UnlockSeqBuf (h_cam2, (rightImgNum), (this->act_img_buf2));
		if (retVal != IS_SUCCESS)
		{
			printf("is_UnlockSeqBuf for active buffer Failed (Code: %d)\n", retVal);
			return retVal;
		}

		prevImgNum = rightImgNum -1;
		if (rightImgNum == 1)
			prevImgNum = imgBufferCount;
/*
		// Unlock last image buffer for camera 2
//		retVal = is_UnlockSeqBuf (h_cam2, IS_IGNORE_PARAMETER, (this->last_img_buf2));
		retVal = is_UnlockSeqBuf (h_cam2, prevImgNum, (this->last_img_buf2));
		if (retVal != IS_SUCCESS)
		{
			printf("is_UnlockSeqBuf for last buffer Failed (Code: %d)\n", retVal);
			return retVal;
		}
		*/
	}
	/*
	retVal = is_WaitEvent(h_cam2, IS_SET_EVENT_FRAME, WAIT_TIMEOUT_MS);
	if (retVal != IS_SUCCESS)
	{
		printf("Is wait event Failed (Code: %d)\n", retVal);
		return retVal;
	}
	*/
	retVal = is_GetActSeqBuf (h_cam2, &rightImgNum, &(this->act_img_buf2), &(this->last_img_buf2));
	if (retVal != IS_SUCCESS)
	{
		printf("GetActSeqBuf Failed (Code: %d)\n", retVal);
		return retVal;
	}
	rightNewImageFrame.data = (uchar*) this->last_img_buf2;


	// lock active image buffer for camera 2, this particular memory will be skipped while camera 2 is running in freerun mode
	retVal = is_LockSeqBuf (h_cam2, rightImgNum, (this->act_img_buf2));
	if (retVal != IS_SUCCESS)
	{
		printf("is_LockSeqBuf for active buffer Failed (Code: %d)\n", retVal);
		return retVal;
	}

	prevImgNum = rightImgNum -1;
	if (rightImgNum == 1)
		prevImgNum = imgBufferCount;
/*
	// lock active last buffer for camera 2, this particular memory will be skipped while camera 2 is running in freerun mode
	retVal = is_LockSeqBuf (h_cam2, prevImgNum, (this->last_img_buf2));
//	retVal = is_LockSeqBuf (h_cam2, IS_IGNORE_PARAMETER, (this->last_img_buf2));
	if (retVal != IS_SUCCESS)
	{
		printf("is_LockSeqBuf for last buffer Failed (Code: %d)\n", retVal);
		return retVal;
	}
*/
	retVal = is_GetActSeqBuf (h_cam1, &leftImgNum, &(this->act_img_buf1), &(this->last_img_buf1));
	if (retVal != IS_SUCCESS)
	{
		printf("GetActSeqBuf Failed (Code: %d)\n", retVal);
		return retVal;
	}
	leftNewImageFrame.data = (uchar*) this->last_img_buf1;

	// get current time in ms for timestamp
//	gettimeofday(&this->timeRAW,NULL);
//	imageTimestamp = timeRAW.tv_sec*1000+timeRAW.tv_usec/1000; // ms
//	imageTimestamp = imageTimestamp - zerotime;


	// check if last_img_buf position has changed
	// if not, current frames are out of synch --> don't change frames (= use previous frames) and return synchCheckFlag = -1
	if(this->last_img_buf1 == bufDummy1 || this->last_img_buf2 == bufDummy2)
	{
		synchCheckFlag = -1;
	}
	else
	{
		leftNewImageFrame.data = (uchar*) this->last_img_buf1;
		rightNewImageFrame.data = (uchar*) this->last_img_buf2;

		synchCheckFlag = 0;

	}

	this->bufDummy1 = this->last_img_buf1;
	this->bufDummy2 = this->last_img_buf2;

	//commented to stop adding to log on loop
	//cout << "Double Imaged Saved" << endl;
	return 0;
}

//// End of Synch functions


//// Begin of NotSynch functions
unsigned int opticsmount_Cameras::initTwoCamerasNotSynch()
{
	int img_xpos, img_ypos, imageWidth, imageHeight;
	int bits_pixel;
	double fps, exp_time;
	int retVal;
	int i;

	memset(&camInfo1,0,sizeof(CAMINFO));
	memset(&camInfo2,0,sizeof(CAMINFO));

	//initialize camera 1 & 2
	retVal = is_InitCamera(&h_cam1,0);
	if (retVal != IS_SUCCESS)
	{
		h_cam1 = 0;
		printf("Init Camera Failed (Code: %d)\n",retVal);
		return retVal;
	}

	retVal = is_InitCamera(&h_cam2,0);
	if (retVal != IS_SUCCESS)
	{
		h_cam2 = 0;
		printf("Init Camera Failed (Code: %d)\n",retVal);
		return retVal;
	}

	//Get camera info 1 & 2
	retVal = is_GetCameraInfo(h_cam1, &camInfo1);
	if (retVal != IS_SUCCESS)
	{
		printf("Get Camera Info Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("S/N of CAM 1: %s - Select: %d\n", camInfo1.SerNo, camInfo1.Select);
	retVal = is_GetCameraInfo(h_cam2, &camInfo2);
	if (retVal != IS_SUCCESS)
	{
		printf("Get Camera Info Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("S/N of CAM 2: %s - Select: %d\n", camInfo2.SerNo, camInfo2.Select);

	for (int count = 0; count < 16; count++) {
		if (OPTICS_ID[count] == -1) {
			cout << "CAMERA S/N Not Found" << endl;
			break;
		}
		if (strcmp(camInfo1.SerNo,LEFT_CAM_SERIAL[count]) == 0) {
			strcpy(this->CAMERA_1_SERIAL, camInfo1.SerNo);
			cout << "LEFT CAM S/N: " << this->CAMERA_1_SERIAL << " (Not Swapped)" << endl;
			cout << "OPTICS MOUNT S/N: " << OPTICS_ID[count] << endl;
			break;
		} else if (strcmp(camInfo2.SerNo,LEFT_CAM_SERIAL[count]) == 0) {
			strcpy(this->CAMERA_1_SERIAL, camInfo2.SerNo);
			cout << "LEFT CAM S/N: " << this->CAMERA_1_SERIAL << " (Swapped)" << endl;
			cout << "OPTICS MOUNT S/N: " << OPTICS_ID[count] << endl;
			HIDS h_cam_temp;
			h_cam_temp = h_cam2;
			h_cam2 = h_cam1;
			h_cam1 = h_cam_temp;
			break;
		}
	}

	//get color modes 1 & 2
	bits_pixel = imgBitsPixel;
	retVal = is_SetColorMode(h_cam1, IS_SET_CM_Y8);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Color Mode Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_SetColorMode(h_cam2, IS_SET_CM_Y8);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Color Mode Failed (Code: %d)\n", retVal);
		return retVal;
	}

	if (reduceImageSizeTo320x240)
	{

		retVal = is_SetBinning(h_cam1, IS_BINNING_2X_VERTICAL || IS_BINNING_2X_HORIZONTAL);
		if (retVal != IS_SUCCESS)
		{
			printf("Set Binning failed for Camera 1 (Code: %d)\n", retVal);
			return retVal;
		}

		retVal = is_SetBinning(h_cam2, IS_BINNING_2X_VERTICAL || IS_BINNING_2X_HORIZONTAL);
		if (retVal != IS_SUCCESS)
		{
			printf("Set Binning failed for Camera 2 (Code: %d)\n", retVal);
			return retVal;
		}

		this->imgHeight = int( (this->imgHeight)/2 );
		this->imgWidth = int( (this->imgWidth)/2 );

	}

	//set area of interest 1 & 2
	if (reduceImageSizeTo320x240)
	{
		img_ypos = 0;
		img_xpos = 28;	//shifts the area of interest so that the image in the center of the ccd
	}
	else
	{
		img_ypos = 0;
		img_xpos = 56;	//shifts the area of interest so that the image in the center of the ccd
	}

	printf("img_xpos = %d\n", img_xpos);

	retVal = is_SetAOI(h_cam1, IS_SET_IMAGE_AOI, &img_xpos, &img_ypos, &(this->imgWidth), &(this->imgHeight));
	if (retVal != IS_SUCCESS)
	{
		printf("SetAOI Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_SetAOI(h_cam2, IS_SET_IMAGE_AOI, &img_xpos, &img_ypos, &(this->imgWidth), &(this->imgHeight));
	if (retVal != IS_SUCCESS)
	{
		printf("SetAOI Failed (Code: %d)\n", retVal);
		return retVal;
	}

	for(i = 0; i < imgBufferCount; i++)
	{
		//cam 1
		//allocate image memory
		retVal = is_AllocImageMem(h_cam1, this->imgWidth, this->imgHeight, bits_pixel, &Cam1_ringbuffer[i].pBuf, &Cam1_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("AllocImageMem Failed (Code: %d)\n", retVal);
			return retVal;
		}

		//add to sequence
		retVal = is_AddToSequence(h_cam1, Cam1_ringbuffer[i].pBuf, Cam1_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("AddToSequence Failed (Code: %d)\n", retVal);
			return retVal;
		}
		//printf("Image Buffer Address(%d): 0x%08X\n",i, Cam1_ringbuffer[i].pBuf);

		//cam 2
		//allocate image memory
		retVal = is_AllocImageMem(h_cam2, this->imgWidth, this->imgHeight, bits_pixel, &Cam2_ringbuffer[i].pBuf, &Cam2_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("AllocImageMem Failed (Code: %d)\n", retVal);
			return retVal;
		}

		//add to sequence
		retVal = is_AddToSequence(h_cam2, Cam2_ringbuffer[i].pBuf, Cam2_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("AddToSequence Failed (Code: %d)\n", retVal);
			return retVal;
		}
		//printf("Image Buffer Address(%d): 0x%08X\n",i, Cam2_ringbuffer[i].pBuf);
	}

	//set up frame rate 1 & 2
	retVal = is_SetFrameRate(h_cam1, frameRate, &fps);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Frame Rate Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("New Frame rate: %f\n",fps);
	retVal = is_SetFrameRate(h_cam2, frameRate, &fps);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Frame Rate Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("New Frame rate: %f\n",fps);

	//set gain value
	retVal = is_SetGainBoost(h_cam1, IS_SET_GAINBOOST_OFF);
	if (retVal != IS_SUCCESS)
	{
		printf("Enable GainBoost Failed (Code: %d)\n", retVal);
		return retVal;
	}

	retVal = is_SetGainBoost(h_cam2, IS_SET_GAINBOOST_OFF);
	if (retVal != IS_SUCCESS)
	{
		printf("Enable GainBoost Failed (Code: %d)\n", retVal);
		return retVal;
	}

	retVal = is_SetHardwareGain(h_cam1, this->hwGain, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
	if (retVal != IS_SUCCESS)
	{
		printf("Set HW Gain Failed (Code: %d)\n", retVal);
		return retVal;
	}
//	retVal = is_SetHardwareGain(h_cam1, IS_GET_MASTER_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
//	printf("New Gain Factor: %d\n", retVal);

	retVal = is_SetHardwareGain(h_cam2, this->hwGain, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
	if (retVal != IS_SUCCESS)
	{
		printf("Set HW Gain Failed (Code: %d)\n", retVal);
		return retVal;
	}
//	retVal = is_SetHardwareGain(h_cam2, IS_GET_MASTER_GAIN, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER, IS_IGNORE_PARAMETER);
//	printf("New Gain Factor: %d\n", retVal);


	//set up exposure time 1 & 2
	retVal = is_SetExposureTime(h_cam1, exposureTime, &exp_time);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Exposure Time Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("New Exposure Time: %f\n",exp_time);
	retVal = is_SetExposureTime(h_cam2, exposureTime, &exp_time);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Exposure Time Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("New Exposure Time: %f\n",exp_time);

	//set up flash strobe 1 & 2
	retVal = is_SetFlashStrobe(h_cam1, IS_SET_FLASH_HI_ACTIVE_FREERUN, 0);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Flash Strobe Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_SetFlashStrobe(h_cam2, IS_SET_FLASH_HI_ACTIVE_FREERUN, 0);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Flash Strobe Failed (Code: %d)\n", retVal);
		return retVal;
	}

	//set up flash delay 1 & 2
	retVal = is_SetFlashDelay(h_cam1, flashDelay, flashDuration);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Flash Delay Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_SetFlashDelay(h_cam2, flashDelay, flashDuration);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Flash Delay Failed (Code: %d)\n", retVal);
		return retVal;
	}

	//Enable frame event
	retVal = is_EnableEvent(h_cam1, IS_SET_EVENT_FRAME);        // enable frame event
	if (retVal != IS_SUCCESS)
	{
		printf("Enable Event Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_EnableEvent(h_cam2, IS_SET_EVENT_FRAME);        // enable frame event
	if (retVal != IS_SUCCESS)
	{
		printf("Enable Event Failed (Code: %d)\n", retVal);
		return retVal;
	}

	return 0;
}

unsigned int opticsmount_Cameras::closeTwoCamerasNotSynch()
{
	int i, retVal;

	for(i = 0; i < imgBufferCount; i++)
	{
		//deallocate image memory
		retVal = is_FreeImageMem(h_cam1, Cam1_ringbuffer[i].pBuf, Cam1_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("FreeImageMem Failed (Code: %d)\n", retVal);
			return retVal;
		}
		Cam1_ringbuffer[i].pBuf = NULL;
		Cam1_ringbuffer[i].img_id = 0;

		retVal = is_FreeImageMem(h_cam2, Cam2_ringbuffer[i].pBuf, Cam2_ringbuffer[i].img_id);
		if (retVal != IS_SUCCESS)
		{
			printf("FreeImageMem Failed (Code: %d)\n", retVal);
			return retVal;
		}
		Cam2_ringbuffer[i].pBuf = NULL;
		Cam2_ringbuffer[i].img_id = 0;

	}

	//close camera 1 & 2
	if(h_cam1)
	{
		is_ExitCamera(h_cam1);
	}
	h_cam1 = 0;
	if(h_cam2)
	{
		is_ExitCamera(h_cam2);
	}
	h_cam2 = 0;

	return 0;
}

unsigned int opticsmount_Cameras::startTwoCamerasNotSynch()
{
	int retVal;

	//start capture 1 & 2
	retVal = is_CaptureVideo(h_cam1, IS_DONT_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Capture Video Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_CaptureVideo(h_cam2, IS_DONT_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Capture Video Failed (Code: %d)\n", retVal);
		return retVal;
	}
	
	return 0;
}

unsigned int opticsmount_Cameras::stopTwoCamerasNotSynch()
{
	int retVal;

	retVal = is_StopLiveVideo(h_cam1, IS_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Stop Live Video Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_StopLiveVideo(h_cam2, IS_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Stop Live Video Failed (Code: %d)\n", retVal);
		return retVal;
	}

	return 0;
}

unsigned int opticsmount_Cameras::captureTwoImagesNotSynch(cv::Mat& leftNewImageFrame, cv::Mat& rightNewImageFrame, int* img_num1, int* img_num2)
{
	int retVal;

	//this function is not documented in the programmers manual
	//it is mentioned in README.TXT and used in the demo program...
	//the parameters are (camera_handle, event_type, timeout_in_ms)

	retVal = is_WaitEvent(h_cam1, IS_SET_EVENT_FRAME, WAIT_TIMEOUT_MS);
	if (retVal != IS_SUCCESS)
	{
		printf("Is wait event Failed (Code: %d)\n", retVal);
		return retVal;
	}
	retVal = is_WaitEvent(h_cam2, IS_SET_EVENT_FRAME, WAIT_TIMEOUT_MS);
	if (retVal != IS_SUCCESS)
	{
		printf("Is wait event Failed (Code: %d)\n", retVal);
		return retVal;
	}

	retVal = is_GetActSeqBuf(h_cam1, img_num1, &(this->act_img_buf1), &(this->last_img_buf1));
	if (retVal != IS_SUCCESS)
	{
		printf("GetActSeqBuf Failed (Code: %d)\n", retVal);
		return retVal;
	}

	retVal = is_GetActSeqBuf(h_cam2, img_num2, &(this->act_img_buf2), &(this->last_img_buf2));
	if (retVal != IS_SUCCESS)
	{
		printf("GetActSeqBuf Failed (Code: %d)\n", retVal);
		return retVal;
	}

	leftNewImageFrame.data = (uchar*) this->last_img_buf1;
	rightNewImageFrame.data = (uchar*) this->last_img_buf2;

	// get current time in ms for timestamp
//	gettimeofday(&this->timeRAW,NULL);
//	imageTimestamp = timeRAW.tv_sec*1000+timeRAW.tv_usec/1000; // ms
//	imageTimestamp = imageTimestamp - zerotime;

	cout << "Double Image Saved" << endl;
	return 0;
}
//// end of NotSynch cameras functions
