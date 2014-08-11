#include "videoStreaming.h"
#include "networking.h"
#include "haloGSP.h"

//////////////////////////////////
//////////// Video Streaming Class

VideoStreaming::VideoStreaming()
{
	pthread_mutex_init(&VideoBufferMutex, NULL);
	pthread_mutex_unlock(&VideoBufferMutex);

	pthread_mutex_init(&VideoModeListSendMutex, NULL);
	pthread_mutex_unlock(&VideoModeListSendMutex);

	network = new NetworkingClass();

	videoStreamingOn = true;

//	videoStreamingModeFlag = 1;

	numberOfVideoStreamingModes = 0; // default VideoMode will be setup from GSP init function
	numberOfOpenGLstreamingModes = 0;

	newFrameToSend = false;

	noVideo = false;

	pthread_mutex_lock(&VideoModeListSendMutex);
	sendVideoList = false;
	pthread_mutex_unlock(&VideoModeListSendMutex);

	this->currentModeType = MAT_VID_BUFF_TYPE;
	this->defaultVideoMode = -1;

	defaultModeOn = true;
}

void VideoStreaming::initVideoStreaming(haloGSP *gspProgram)
{
	this->program = gspProgram;

	// sets "current" values to values in first pointer entry, which corresponds to the default video mode
//	this->currentModeName = this->videoStreamingBufferList[0]->videoStreamingModeName;

//	this->currentModeNumber = this->videoStreamingBufferList[0]->modeNumber;

//s	this->videoStreamingBufferList[0]->active = true;

//	this->defaultVideoMode = 1;

	if (this->defaultVideoMode == -1) 
	{
		this->setAsDefaultVideoMode(*(this->videoStreamingBufferList[0]));
	}

//	this->setVideoStreamingMode(this->defaultVideoMode);

//	this->videoStreamingModeFlag = 1;
	cout << "before network" << endl;
	this->network->initNetworking();
	cout << "after network" << endl;
}

void VideoStreaming::setIPAddrPort(string & server_ip, int & port) 
{
	this->network->server_ip = server_ip;
	this->network->port = port;
}

void VideoStreaming::createNew_MatVideoBuffer(MatVideoBuffer& newBuffer, string nameOfStreamingMode)
{
	newBuffer.videoStreamingModeName = nameOfStreamingMode;
	newBuffer.active = false;

	this->addVideoBufferToBufferManager(&newBuffer);
}

void VideoStreaming::createNew_OpenGLvideoBuffer(OpenGLbuffer& newbuffer, int numframes, string nameOfStreamingMode)
{
	newbuffer.videoStreamingModeName = nameOfStreamingMode;
	newbuffer.active = false;

	this->addVideoBufferToBufferManager(&newbuffer);
}

void VideoStreaming::addVideoBufferToBufferManager(MatVideoBuffer* newBuffer)
{
	VideoModeMapType newEntry;

	this->videoBufferManager.numberOfOpenCVstreamingModes++;
	newEntry.modeNum = videoBufferManager.numberOfOpenCVstreamingModes + videoBufferManager.numberOfOpenGLstreamingModes;
	newEntry.vecPos = videoBufferManager.numberOfOpenCVstreamingModes - 1;
	newEntry.modeType = MAT_VID_BUFF_TYPE;
	newEntry.modeName = newBuffer->videoStreamingModeName;
	this->videoBufferManager.videoModeMap.push_back(newEntry); // new entry in videoModeMap

	this->videoBufferManager.totalNumberOfModes = videoBufferManager.numberOfOpenCVstreamingModes + videoBufferManager.numberOfOpenGLstreamingModes;

	this->videoStreamingBufferList.resize(videoBufferManager.numberOfOpenCVstreamingModes);
	this->videoStreamingBufferList[this->videoStreamingBufferList.size() - 1] = newBuffer; // add buffer address to list
}

void VideoStreaming::addVideoBufferToBufferManager(OpenGLbuffer* newBuffer)
{
	VideoModeMapType newEntry;

	this->videoBufferManager.numberOfOpenGLstreamingModes++;
	newEntry.modeNum = videoBufferManager.numberOfOpenCVstreamingModes + videoBufferManager.numberOfOpenGLstreamingModes;
	newEntry.vecPos = videoBufferManager.numberOfOpenGLstreamingModes - 1;
	newEntry.modeType = OPENGL_VID_BUFF_TYPE;
	newEntry.modeName = newBuffer->videoStreamingModeName;
	this->videoBufferManager.videoModeMap.push_back(newEntry); // new entry in videoModeMap

	this->videoBufferManager.totalNumberOfModes = videoBufferManager.numberOfOpenCVstreamingModes + videoBufferManager.numberOfOpenGLstreamingModes;

	this->openGLstreamingBufferList.resize(videoBufferManager.numberOfOpenGLstreamingModes);
	this->openGLstreamingBufferList[this->openGLstreamingBufferList.size() - 1] = newBuffer; // add buffer address to list
}

void VideoStreaming::update_MatVideoBuffer(MatVideoBuffer& bufferToUpdate, cv::Mat& frame)
{
	if (this->videoStreamingOn) 
	{
		pthread_mutex_lock(&VideoBufferMutex);
		bufferToUpdate.frame = frame.clone();
		if(bufferToUpdate.active) 
		{
			newFrameToSend = true;
		}
		pthread_mutex_unlock(&VideoBufferMutex);
	}
}

void VideoStreaming::update_OpenGLBuffer(OpenGLbuffer& bufferToUpdate, GLenum modeIN, unsigned int indicesIN[], float xyzIN[], unsigned char rgbIN[])
{
	if (this->videoStreamingOn) 
	{
		pthread_mutex_lock(&VideoBufferMutex);
		bufferToUpdate.mode = modeIN;
		bufferToUpdate.indices = *indicesIN;
		bufferToUpdate.xyz = *xyzIN;
		bufferToUpdate.rgb = *rgbIN;
		if(bufferToUpdate.active) 
		{
			newFrameToSend = true;
		}
		pthread_mutex_unlock(&VideoBufferMutex);
	}
}

void VideoStreaming::sendVideoModes() 
{
	pthread_mutex_lock(&VideoModeListSendMutex);
	sendVideoList = true;
	pthread_mutex_unlock(&VideoModeListSendMutex);
}

void VideoStreaming::receiveMessages() 
{
	network->receiveMessages(this);
}

void VideoStreaming::processMouseClick(int x, int y) {
	this->program->GSprocessGuiMouseClick(x,y);
}

void VideoStreaming::processKeyPress(unsigned char key) {
	this->program->GSprocessGuiKeyPress(key);
}

unsigned int VideoStreaming::close() {
	network->closeNetworking();
}

void VideoStreaming::setVideoStreamingMode(int videoMode)
{
	if (videoMode == NO_VIDMODE) 
	{
		noVideo = true;
		return;
	} 
	else 
	{
		noVideo = false;
	}

	if (currentModeType == MAT_VID_BUFF_TYPE) 
	{
		this->videoStreamingBufferList[posInCorrBufList]->active = false;
	} 
	else 
	{
		this->openGLstreamingBufferList[posInCorrBufList]->active = false;
	}

	if (videoMode > this->videoBufferManager.totalNumberOfModes || videoMode < 1)
	{
		printf("ERROR: Specified Videostreaming Mode [%d] does not exist! Keeping previous Videostreaming Mode\n", videoMode);
	}
	else
	{
//		this->videoStreamingModeFlag = videoMode;
		if (this->videoBufferManager.videoModeMap[videoMode-1].modeType == MAT_VID_BUFF_TYPE) // openCV image buffer
		{
			posInCorrBufList = this->videoBufferManager.videoModeMap[videoMode-1].vecPos;
			currentModeType = this->videoBufferManager.videoModeMap[videoMode-1].modeType;
			currentModeNumber = this->videoBufferManager.videoModeMap[videoMode-1].modeNum;
			currentModeName = this->videoStreamingBufferList[posInCorrBufList]->videoStreamingModeName;
			this->videoStreamingBufferList[posInCorrBufList]->active = true;

		}
		else // openGL 3D image buffer
		{
			posInCorrBufList = this->videoBufferManager.videoModeMap[videoMode-1].vecPos;
			currentModeType = this->videoBufferManager.videoModeMap[videoMode-1].modeType;
			currentModeNumber = this->videoBufferManager.videoModeMap[videoMode-1].modeNum;
			currentModeName = this->openGLstreamingBufferList[posInCorrBufList]->videoStreamingModeName;
			this->openGLstreamingBufferList[posInCorrBufList]->active = true;

		}

//		printf("Videostreaming Mode set to : %d\n", this->videoStreamingModeFlag);
	}
}

int VideoStreaming::setAsDefaultVideoMode(BufferBaseClass & buffer) 
{
	bool found = false;
	for (int mode = 1; mode <= this->videoBufferManager.totalNumberOfModes; mode++) {
		if (this->videoBufferManager.videoModeMap[mode-1].modeName == buffer.videoStreamingModeName) 
		{
			//mode is found
			defaultVideoMode = mode;
			found = true;
			break;
		}
	}
	if (found == false) 
	{
		printf("Error: Video Mode not Registered\n");
		return -1;
	}

	if (defaultModeOn == true) 
	{
		this->setVideoStreamingMode(defaultVideoMode);
	}
	return 0;
}

int VideoStreaming::setAsDefaultVideoMode() 
{
	defaultVideoMode = 1;
	if (defaultModeOn == true) 
	{
		this->setVideoStreamingMode(defaultVideoMode);
	}
	return 0;

}

void VideoStreaming::activateDefaultVideoMode() 
{
	if (this->defaultModeOn == false) 
	{
		this->defaultModeOn = true;
		this->setVideoStreamingMode(this->defaultVideoMode);
	}
}
void VideoStreaming::activateStandardVideoMode(int mode) 
{
	if (this->defaultModeOn == true) 
	{
		this->defaultModeOn = false;
		this->setVideoStreamingMode(mode);
	} 
	else 
	{
		this->setVideoStreamingMode(mode);
	}

}

void VideoStreaming::sendVideoStream()
{
	if (this->videoStreamingOn) 
	{
		pthread_mutex_lock(&VideoModeListSendMutex);
		if (sendVideoList == true) 
		{
			sendVideoList = false;
			pthread_mutex_unlock(&VideoModeListSendMutex);
			network->sendVideoModesList(videoBufferManager);
		} 
		else 
		{
			pthread_mutex_unlock(&VideoModeListSendMutex);
		}

		if (newFrameToSend && !noVideo) 
		{
			pthread_mutex_lock(&VideoBufferMutex);
			if (currentModeType == MAT_VID_BUFF_TYPE)
			{
				network->sendMatFrame(this->videoStreamingBufferList[posInCorrBufList]->frame, currentModeNumber);
//				imshow("Test Project",this->videoStreamingBufferList[posInCorrBufList]->frame);
			}
			else if (currentModeType == OPENGL_VID_BUFF_TYPE)
			{
				printf("openGl dummy not implemented\n");
			} 
			else 
			{
				printf("Type not recognized by video streamer\n");
			}
			pthread_mutex_unlock(&VideoBufferMutex);
			newFrameToSend = false;
		}

	}
}

bool VideoStreaming::checkVideoModeActive(BufferBaseClass & testBuffer) 
{
	if (currentModeName == testBuffer.videoStreamingModeName) 
	{
		return true;
	} 
	else 
	{
		cout << currentModeName << ", " << testBuffer.videoStreamingModeName << endl;
		return false;
	}
}
