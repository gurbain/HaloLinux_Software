#ifndef _NETWORKING_H
#define _NETWORKING_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <iostream>

#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "opencv2/core/core.hpp"

#include "videoStreaming.h"

using namespace std;

#define DEFAULT_IP_ADDRESS	"127.0.0.1"
#define DEFAULT_PORT			4000
#define IMAGE_BUFFER_SIZE 640*480*4*3

//******************
//NETWORK PROTOCOL

//msg types to Video Display
#define MODE_LIST_TYPE	0xFF000101
#define MAT_MSG_TYPE	0xFF001001

//msg types to Goggles
#define NEW_MODE_SEL	0xFF000202
#define MOUSE_CLICK		0xFF002002
#define KEY_PRESS		0xFF020002

//default video mode
#define DEFAULT_VIDMODE	0x7F000303
#define NO_VIDMODE		0x7F003003

//color types
#define COLOR_RGB		0xF1

typedef struct {
	int width;
	int height;
	int modeNumber;
	unsigned int color_flag;
} matMsgHeader;

typedef struct {
	int modeNumber;
	int modeType;
	char name[32];
} videoModeDef;

typedef struct {
	int x;
	int y;
} mouseClick;

typedef struct {
	unsigned int key;
} pressedKey;
//******************

class NetworkingClass {
	unsigned char image_buffer[IMAGE_BUFFER_SIZE];
	int sockfd;//, newsockfd;
	struct sockaddr_in servaddr;//, cliaddr;

public:
	string server_ip;
	int port;

	NetworkingClass() {
		server_ip = DEFAULT_IP_ADDRESS;
		port = DEFAULT_PORT;
	};

	int initNetworking();
	int closeNetworking();
	int sendMatFrame(cv::Mat & frame, int modeNum);
	int sendOpenGLFrame();
	int sendVideoModesList(VideoBufferManager & manager);
	int receiveMessages(VideoStreaming * stream);

	void setIPAddrPort(string server_ip, int port);

private:
	int processNewModeMessage();
	mouseClick processMouseClick();
	pressedKey processKeyPress();
};


#endif
