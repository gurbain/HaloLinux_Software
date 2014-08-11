#include "networkServer.h"

int sockfd, newsockfd;
struct sockaddr_in servaddr, cliaddr;


int NetworkServerClass::init() {
	int retVal;
	socklen_t clilen;
	pthread_t networkHandlerThread;

	videoDisplay.init();

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Socket Open Fail: %s\n", strerror(errno));
		return 1;
	}

	memset(&servaddr, '\0', sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	retVal = bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (retVal == -1)
	{
		printf("Bind Error: %s\n", strerror(errno));
		return -1;
	}

	retVal = listen(sockfd, 5);
	if (retVal == -1)
	{
		printf("Listen Error: %s\n", strerror(errno));
		return 1;
	}

	clilen = sizeof(cliaddr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cliaddr, &clilen);
	if (newsockfd == -1)
	{
		printf("Accept Error: %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

int NetworkServerClass::closeServer() {
	close(sockfd);
	close(newsockfd);
	return 0;
}

int NetworkServerClass::receiveMessages() {
	int msgLen;

	unsigned int msgType;

	//receive message type
	if (-1 == this->receiveBuffer((char*) &msgType, sizeof(msgType)) ) {
		return -1; }

	switch (msgType) {
	case MODE_LIST_TYPE:
		processModeMessage();
		break;
	case MAT_MSG_TYPE:
		processMatMessage();
		break;
	default:
		printf("Bad Message Type: NetworkServerClass::receiveMessages(): %X\n", msgType);
		return -1;
		break;
	}
	return 0;

}

int NetworkServerClass::processModeMessage() {
	int i, bytesread, retVal;
	videoModeDef modeDef;

	//read number of modes
	int totalModes;
	if (-1 == this->receiveBuffer((char*) &totalModes, sizeof(totalModes)) ) {
		return -1; }
	videoDisplay.clearPopUpMenu();

	//read modes
	//openCV Modes
	for (i = 0; i < totalModes; i++) {
		if (-1 == this->receiveBuffer((char*) &modeDef, sizeof(modeDef)) ) {
			return -1; }
		videoModes.push_back(modeDef);
	}
	videoDisplay.refreshPopUpMenu();
}

int NetworkServerClass::processMatMessage() {
	matMsgHeader matHeader;
	int retVal;
	long int bytesread;
	long int imageBufferSize;

	//receive matheader
	if (-1 == this->receiveBuffer((char*) &matHeader, sizeof(matHeader)) ) {
		return -1; }

	//store height and width and check if too big
	imgHeight = matHeader.height;
	imgWidth = matHeader.width;

	if (imgHeight > IMAGE_HEIGHT || imgWidth > IMAGE_WIDTH) {
		printf("Error: image too big: [%d, %d]\n", imgHeight, imgWidth);
		return -1;
	}

	//printf("Image Mode Number: %d\n", matHeader.modeNumber);

	if (matHeader.color_flag == COLOR_RGB) {
		imageBufferSize = matHeader.width * matHeader.height * 3;
	} else {
		printf("Unrecognizied Color Type\n");
		return -1;
	}

	if (-1 == this->receiveBuffer((char*) &image_buffer, imageBufferSize) ) {
		return -1; }

	videoDisplay.displayImage(image_buffer);
	return 0;
}

int NetworkServerClass::receiveBuffer(char* buff, size_t length) {
	int bytesread = 0;
	int retVal;
	while (bytesread < length)
	{
		retVal = recv(newsockfd, &buff[bytesread], length - bytesread, 0);
		if (retVal == -1)
		{
			printf("Recv error: %s\n", strerror(errno));
			return -1;
		}
		bytesread+=retVal;
	}

	return bytesread;
}
