#include "dockingport_Camera.h"

unsigned int dockingport_Camera::initOneCamera() 
{
	int img_xpos, img_ypos, img_width, img_height;
	int bits_pixel;
	double fps, exp_time;
	int retVal;
	int i;

	memset(&camInfo1,0,sizeof(CAMINFO));
	this->maxFrameRate = float(this->pixelClockFreq * 1000000) / float((752*480));
	if (this->frameRate > this->maxFrameRate)
	{
		printf("Specified FrameRate is higher than camera pixel clock frequency allows! --- frameRate will be set to maximally possible value of %f\n", this->maxFrameRate);
		this->frameRate = this->maxFrameRate;
	}

	//initialize camera 1
	retVal = is_InitCamera(&h_cam1, 0);
	if (retVal != IS_SUCCESS)
	{
		h_cam1 = 50;
		printf("Init Camera Failed (Code: %d)\n",retVal);
		return retVal;
	}

	//Get camera info 1
	retVal = is_GetCameraInfo(h_cam1, &camInfo1);
	if (retVal != IS_SUCCESS)
	{
		printf("Get Camera Info Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("S/N of CAM 1: %s - Select: %d\n", camInfo1.SerNo, camInfo1.Select);

	//get color modes 1 & 2
	bits_pixel = this->imgBitsPixel*3; //Duncan Edit. need to multiply by 3 for rgb
	retVal = is_SetColorMode(h_cam1, IS_CM_BGR8_PACKED);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Color Mode Failed (Code: %d)\n", retVal);
		return retVal;
	}

	//set area of interest 1 & 2
	img_width = this->imgWidth;
	img_height = this->imgHeight;
/*
	retVal = is_SetAOI(h_cam1, IS_SET_IMAGE_AOI, &img_xpos, &img_ypos, &img_width, &img_height);
	if (retVal != IS_SUCCESS)
	{
		printf("SetAOI Failed (Code: %d)\n", retVal);
		return retVal;
	}
 // Adding recommended new function for AOI for VGA
	IS_RECT rectAOI;
	rectAOI.s32X     = 0;
	rectAOI.s32Y     = 0;
	rectAOI.s32Width = img_width;
	rectAOI.s32Height = img_height;

	retVal = is_AOI( h_cam1, IS_AOI_IMAGE_SET_AOI, (void*)&rectAOI, sizeof(rectAOI));
	if (retVal != IS_SUCCESS)
	{
		printf("SetAOI Failed (Code: %d)\n", retVal);
		return retVal;
	}
*/
	int img_id = 13;
	retVal = is_ImageFormat(h_cam1, IMGFRMT_CMD_SET_FORMAT, &img_id, 4);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Image Format Failed (Code: %d)\n", retVal);
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

	}

	//set up frame rate 1 & 2
	retVal = is_SetFrameRate(h_cam1, this->frameRate, &fps);
	if (retVal != IS_SUCCESS)
	{
		printf("Set Frame Rate Failed (Code: %d)\n", retVal);
		return retVal;
	}
	printf("New Frame rate: %f\n",fps);

	//Enable frame event
	retVal = is_EnableEvent(h_cam1, IS_SET_EVENT_FRAME);        // enable frame event
	if (retVal != IS_SUCCESS)
	{
		printf("Enable Event Failed (Code: %d)\n", retVal);
		return retVal;
	}

	return 0;
}

unsigned int dockingport_Camera::startOneCamera()
{
	int retVal;

	retVal = is_CaptureVideo(h_cam1, IS_DONT_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Capture Video Failed (Code: %d)\n", retVal);
		return retVal;
	}

	return 0;
}

unsigned int dockingport_Camera::captureOneImage(cv::Mat& newImage)
{
	int retVal;

	iterDummy++;

	//this function is not documented in the programmers manual
	//it is mentioned in README.TXT and used in the demo program...
	//the parameters are (camera_handle, event_type, timeout_in_ms)

	retVal = is_WaitEvent(h_cam1, IS_SET_EVENT_FRAME, WAIT_TIMEOUT_MS);
	if (retVal != IS_SUCCESS)
	{
		printf("Is wait event Failed (Code: %d)\n", retVal);
		return retVal;
	}

	retVal = is_GetActSeqBuf (h_cam1, &singleImgNum, &(this->act_img_buf1), &(this->last_img_buf1));
	if (retVal != IS_SUCCESS)
	{
		printf("GetActSeqBuf Failed (Code: %d)\n", retVal);
		return retVal;
	}
	newImage.data = (uchar*) this->last_img_buf1;

	//Duncan edit.
	cv::Size s = newImage.size();
	int rows = s.height;
	int cols = s.width;
	//commented to stop from adding to log	
	//cout << "Single Image Saved" << endl;

	// get current time in ms for timestamp
//	gettimeofday(&this->timeRAW,NULL);
//	imageTimestamp = timeRAW.tv_sec*1000+timeRAW.tv_usec/1000; // ms
//	imageTimestamp = imageTimestamp - zerotime;

	return 0;
}

unsigned int dockingport_Camera::stopOneCamera()
{
	int retVal;

	retVal = is_StopLiveVideo(h_cam1, IS_WAIT);
	if (retVal != IS_SUCCESS)
	{
		printf("Stop Live Video Failed (Code: %d)\n", retVal);
		return retVal;
	}

	return 0;
}

unsigned int dockingport_Camera::closeOneCamera()
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

	}

	//close camera 1
	if(h_cam1)
	{
		is_ExitCamera(h_cam1);
	}
	h_cam1 = 50;

	return 0;
}

int dockingport_Camera::getImageWidth()
{
	return this->imgWidth;
}

int dockingport_Camera::getImageHeight()
{
	return this->imgHeight;
}

double dockingport_Camera::getFrameRate()
{
	if (this->useCamera) {
		int retVal;
		double fr;

		retVal = is_SetFrameRate(h_cam1, IS_GET_FRAMERATE, &fr);
		if (retVal != IS_SUCCESS)
		{
			printf("Could not read Frame Rate from Camera (Code: %d)\n", retVal);
			return retVal;
		}

		return fr;
	} else {
		return this->frameRate;
	}

}

void dockingport_Camera::setFrameRate(float newFR)
{
	this->frameRate = newFR;
}

double dockingport_Camera::getExposureTime()
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

void dockingport_Camera::setExposureTime(int newET)
{
	this->exposureTime = newET;
}

void dockingport_Camera::setHWGain(int gain)
{
	this->hwGain = gain;
}


char* dockingport_Camera::getCam1Serial()
{
	return this->CAMERA_1_SERIAL;
}

void dockingport_Camera::setCam1Serial(const char* serialNum)
{
	sprintf(this->CAMERA_1_SERIAL, "%s", serialNum);
}

void dockingport_Camera::sethCam(HIDS hcam)
{
	h_cam1 = hcam;
}
