#ifndef _VIDEO_STREAMING_H
#define _VIDEO_STREAMING_H

#define MAT_VID_BUFF_TYPE		0x01
#define OPENGL_VID_BUFF_TYPE	0x02

// OpenCV
#include "opencv2/core/core.hpp"

// OpenGL
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>

// C++ standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

using namespace std;

// Define classes here to avoid circular dependencies (only pointers used)
// Include statement is in the .cpp file
class haloGSP;
class NetworkingClass;

class BufferBaseClass {

	friend class VideoStreaming;

public:

	string videoStreamingModeName;
	int modeNumber;
	bool active;

	BufferBaseClass()
	{

	}

};

class MatVideoBuffer : public BufferBaseClass{

public:

	cv::Mat frame;

	MatVideoBuffer()
	{

	}
};


class OpenGLbuffer : public BufferBaseClass {

public:

	GLenum mode;			//drawing mode
	unsigned int indices;		//index array
	float xyz;				//X, Y and Z location
	unsigned char rgb;		//color for vertexes
	//this corresponds to the inputs for a glDrawElements() OpenGL call.

	OpenGLbuffer()
	{

	}

};


struct VideoModeMapType {
	int modeNum;
	int vecPos;
	uchar modeType;
	string modeName;
};

class VideoBufferManager {

public:

	vector<VideoModeMapType> videoModeMap;

	int totalNumberOfModes;
	int numberOfOpenCVstreamingModes;
	int numberOfOpenGLstreamingModes;

	VideoBufferManager()
	{
		numberOfOpenCVstreamingModes = 0;
		numberOfOpenGLstreamingModes = 0;
		totalNumberOfModes = 0;
	}

};


class VideoStreaming {

	NetworkingClass* network;

	// Mutex
	pthread_mutex_t VideoBufferMutex;
	pthread_mutex_t VideoModeListSendMutex;

	VideoBufferManager videoBufferManager;

//	int videoStreamingModeFlag;
	int numberOfVideoStreamingModes; // default VideoMode will be setup from GSP init function
	int numberOfOpenGLstreamingModes;

	//default video mode
	bool defaultModeOn;
	int defaultVideoMode;

	bool noVideo;

	bool sendVideoList;

	uchar currentModeType;
	int currentModeNumber;
	string currentModeName;

	bool newFrameToSend;

	// for testing
	char key;

	// vector of VideoBuffer pointers
	vector<MatVideoBuffer*> videoStreamingBufferList;
	vector<OpenGLbuffer*> openGLstreamingBufferList;
	int posInCorrBufList;

	haloGSP *program;

public:
	bool videoStreamingOn;

	VideoStreaming();

	void activateDefaultVideoMode();

	void activateStandardVideoMode(int mode);

	int setAsDefaultVideoMode(BufferBaseClass & buffer);

	int setAsDefaultVideoMode();

	void setIPAddrPort(string & server_ip, int & port);

	void createNew_MatVideoBuffer(MatVideoBuffer& newBuffer, string nameOfStreamingMode);

	void createNew_OpenGLvideoBuffer(OpenGLbuffer& newbuffer, int numframes, string nameOfStreamingMode);

	void update_MatVideoBuffer(MatVideoBuffer& matVideoBuffer, cv::Mat& leftImage);

	void update_OpenGLBuffer(OpenGLbuffer& openGLbuffer, GLenum modeIN, unsigned int indicesIN[], float xyzIN[], unsigned char rgbIN[]);

	void sendVideoModes();

	void receiveMessages();

	void processMouseClick(int x, int y);

	void processKeyPress(unsigned char key);

	unsigned int close();

	void initVideoStreaming(haloGSP *gspProgram);

	void setVideoStreamingMode(int videoStreamingModeFlag);

	void sendVideoStream();

	bool checkVideoModeActive(BufferBaseClass & testBuffer);

private:

	void addVideoBufferToBufferManager(MatVideoBuffer* newBuffer);

	void addVideoBufferToBufferManager(OpenGLbuffer* newBuffer);

};

#endif
