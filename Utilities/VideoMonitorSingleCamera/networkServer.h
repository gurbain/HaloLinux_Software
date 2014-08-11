#ifndef _NETWORKSERVER_H
#define _NETWORKSERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <vector>
#include <pthread.h>

#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#define DEFAULT_SERVER_IP_ADDRESS	"127.0.0.1" 	//"18.33.6.155"
#define PORT			50001

using namespace std;

extern int sockfd, newsockfd;
extern struct sockaddr_in servaddr, cliaddr;

//******************
//NETWORK PROTOCOL

//msg types to Video Display
#define MODE_LIST_TYPE	0xFF000101
#define MAT_MSG_TYPE	0xFF001001

//msg types to Goggles
#define NEW_MODE_SEL	0xFF000202
#define MOUSE_CLICK		0xFF002002
#define KEY_PRESS		0xFF020002

//video mode defines
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

#include "videoDisplay.h"

class NetworkServerClass {

	unsigned char image_buffer[IMAGE_BUFFER_SIZE];

	VideoDisplayClass videoDisplay;

	int processMatMessage();
	int processModeMessage();

public:

	NetworkServerClass() {

	};

	int init();
	int closeServer();
	int receiveMessages();
	int receiveBuffer(char* buff, size_t length);

};

#endif
