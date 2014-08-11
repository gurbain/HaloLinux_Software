#include "networking.h"
#include "videoStreaming.h"

int NetworkingClass::initNetworking() 
{
	int retVal;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) 
	{
		printf("Socket Open Fail: %s\n", strerror(errno));
		return 1;
	}
	//cout << "after socket" << endl;
	memset(&servaddr, '\0', sizeof(servaddr));
	//cout << "after memset" << endl;
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(server_ip.c_str());
	servaddr.sin_port = htons(port);

	retVal = connect(sockfd,(struct sockaddr *) &servaddr,sizeof(servaddr));
	//cout << "after connect" << endl;
	if (retVal == -1)
	{
		printf("Connect Error: %s\n", strerror(errno));
		return 1;
	}

	return 0;
}

int NetworkingClass::closeNetworking() 
{
	close(sockfd);
}

int NetworkingClass::sendMatFrame(cv::Mat & frame, int modeNum) 
{
	int retVal;
	int image_buffer_size;
	int bytessent;

	unsigned int msgType = MAT_MSG_TYPE;
	matMsgHeader msgHeader;

	msgHeader.height = (int) frame.size().height;
	msgHeader.width = (int) frame.size().width;
	msgHeader.modeNumber = modeNum;

	if (frame.elemSize() == 3) 
	{
		msgHeader.color_flag = COLOR_RGB;
	} 
	else 
	{
		cout << "MsgSize Error: " << frame.elemSize() << endl;
		return -1;
	}

//	printf("Width, Height, Color: [%d, %d, %d]\n", msgHeader.width, msgHeader.height, msgHeader.color_flag);

	image_buffer_size = frame.size().height*frame.size().width*frame.elemSize();

	//send type
	retVal = send(sockfd, (void*) &msgType, sizeof(msgType), 0);
	if (retVal == -1 || retVal != sizeof(msgType))
	{
		printf("Send error: %s\n", strerror(errno));
		return -1;
	}
//	printf("Send Mat MsgType: %d [%X]\n", retVal, msgType);

	//send header
	retVal = send(sockfd, (void*) &msgHeader, sizeof(msgHeader), 0);
	if (retVal == -1 || retVal != sizeof(msgHeader))
	{
		printf("Send error: %s\n", strerror(errno));
		return -1;
	}

//	printf("Send Mat MsgHead: %d\n", bytessent);

	//send data
	bytessent = 0;
	while (bytessent < image_buffer_size) 
	{
		retVal = send(sockfd, &frame.data[bytessent], image_buffer_size-bytessent, 0);
		if (retVal == -1)
		{
			printf("Send error: %s\n", strerror(errno));
			return -1;
		}
		bytessent += retVal;
	}
//	printf("Send Mat Frame: %d [%d]\n", bytessent, image_buffer_size);

	return 0;

//	imshow("VideoStream", frame);
}

int NetworkingClass::sendOpenGLFrame() 
{
	return 0;
}

int NetworkingClass::sendVideoModesList(VideoBufferManager& manager) 
{
	int i, retVal, totalModes;
	videoModeDef modeDef;
	totalModes = manager.totalNumberOfModes;

	unsigned int msgType = MODE_LIST_TYPE;
	//send type
	retVal = send(sockfd, (void*) &msgType, sizeof(msgType), 0);
	if (retVal == -1 || retVal != sizeof(msgType))
	{
		printf("Send error: %s\n", strerror(errno));
		return -1;
	}

//	printf("Send Mode MsgType: %d [%X]\n", retVal, msgType);

	//send number
	retVal = send(sockfd, (void*) &totalModes, sizeof(totalModes), 0);
	if (retVal == -1 || retVal != sizeof(totalModes))
	{
		printf("Send error: %s\n", strerror(errno));
		return -1;
	}

//	printf("Send Mat TotalModes: %d\n", retVal);

	//openCV Modes
	for (i = 0; i < totalModes; i++) 
	{
		modeDef.modeNumber = manager.videoModeMap[i].modeNum;
		modeDef.modeType =  manager.videoModeMap[i].modeType;
		strcpy(modeDef.name, manager.videoModeMap[i].modeName.c_str());

		printf("Mode Name: %s, Mode Number: %d, Mode Type: %u\n",
				modeDef.name,
				modeDef.modeNumber,
				modeDef.modeType);

		//send mode
		retVal = send(sockfd, (void*) &modeDef, sizeof(modeDef), 0);
		if (retVal == -1 || retVal != sizeof(modeDef))
		{
			printf("Send error: %s\n", strerror(errno));
			return -1;
		}
//		printf("Send Mat ModeStruct: %d\n", retVal);
	}
}

int NetworkingClass::receiveMessages(VideoStreaming * stream) 
{
	int bytesread;
	int retVal=0;
	unsigned int msgType;

	//receive message type
	retVal = recv(sockfd, &msgType, sizeof(msgType), MSG_DONTWAIT);
	if (retVal == -1 || retVal != sizeof(msgType))
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK) 
		{
			return 0;
		} 
		else 
		{
			printf("Recv error: %s\n", strerror(errno));
			return -1;
		}
	} 
	else 
	{
		switch (msgType) 
		{
		case NEW_MODE_SEL:
			int newMode;
			newMode = processNewModeMessage();
			printf("New Mode: %d\n", newMode);
			if (newMode == DEFAULT_VIDMODE) {
				stream->activateDefaultVideoMode();
			} 
			else 
			{
				stream->activateStandardVideoMode(newMode);
			}
			break;
		case MOUSE_CLICK:
			mouseClick click;
			click = processMouseClick();
			stream->processMouseClick(click.x, click.y);
			break;
		case KEY_PRESS:
			pressedKey key;
			key = processKeyPress();
			stream->processKeyPress(key.key);
			break;
		default:
			printf("Bad Message Type: NetworkingClass::receiveMessages(VideoStreaming*): %X\n", msgType);
			return -1;
			break;
		}
		return 0;
	}
}

pressedKey NetworkingClass::processKeyPress() 
{
	int retVal;
	pressedKey key;
	int bytesread = 0;
	unsigned char charkey;

	while (bytesread < sizeof(key))
	{
		retVal = recv(sockfd, &key, sizeof(key) - bytesread, 0);
		if (retVal == -1)
		{
			printf("Recv error: %s\n", strerror(errno));
			break;
		}
		bytesread += retVal;
	}
	charkey = (unsigned char) key.key;
	//printf("NetworkingClass Key: %c\n", charkey);
	return key;
}

mouseClick NetworkingClass::processMouseClick() 
{
	int retVal;
	mouseClick click;
	int bytesread = 0;

	while (bytesread < sizeof(click))
	{
		retVal = recv(sockfd, &click, sizeof(click) - bytesread, 0);
		if (retVal == -1)
		{
			printf("Recv error: %s\n", strerror(errno));
			break;
		}
		bytesread += retVal;
	}
	return click;
}

int NetworkingClass::processNewModeMessage() 
{
	int newMode, retVal;
	int bytesread = 0;

//	printf("Processing New Mode Message\n");
	while (bytesread < sizeof(newMode))
	{
		retVal = recv(sockfd, &newMode, sizeof(newMode) - bytesread, 0);
		if (retVal == -1)
		{
			printf("Recv error: %s\n", strerror(errno));
			return -1;
		}
		bytesread += retVal;
	}

	return newMode;
}


