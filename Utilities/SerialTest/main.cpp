#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <termios.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

using namespace std;

#define SERIAL_PORT	"/dev/ttyUSB0"		//ttyS0 for pico-itx board, ttyUSB0 for keyspan usb

//message types
typedef struct {
	unsigned int messageID;
	unsigned int spheresHWID;
	unsigned int programID;
} msg_spheres_id;

int serial_fd;
pthread_mutex_t spheres_data_mutex;

unsigned int initSpheres()
{
	int retVal;
	struct termios options;

	//open the serial port
	serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
	if (serial_fd == -1)
	{
		printf("Open Serial Port Failed: %s\n", SERIAL_PORT);
		return -1;
	}
	else
	{
		fcntl(serial_fd, F_SETFL, 0);
	}

	//get termios options
	tcgetattr(serial_fd, &options);

	//set up baud rates
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	//set up number of bits
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= (CS8 | CLOCAL | CREAD);

	//set up no parity checking
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	//disable hardware/software flow control
	options.c_cflag &= ~CRTSCTS;
	//options.c_iflag &= ~(IXON |IXOFF | IXANY | ICRNL);
	options.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

	options.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OLCUC | OPOST);

	//set canonical
//	options.c_lflag &= ~ICANON;		//old
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG); //new


	//set termios options
	tcflush(serial_fd, TCIFLUSH);
	tcsetattr(serial_fd, TCSANOW, &options);


	return 0;
}

unsigned int closeSpheres()
{
	int retVal;

	//close serial port
	close(serial_fd);

	printf("Closed spheres\n");

	return 0;
}


int main (int argc, char *argv[])
{
	int retVal;
	unsigned char requestByte = 0xA1;
	unsigned char retByte;
	int errorCount = 0;

	initSpheres();
	cout << "Hello serial world\n";

	usleep(1000000);

	int seconds = 6*60;
	int rate = 100;

	for (int i=0; i < seconds*rate; i++) {
		retVal = write(serial_fd, (void *)&requestByte, sizeof(requestByte));
		if (retVal < sizeof(requestByte))
		{
			printf("Write failed\n");
			exit(0);
		}

		usleep(1000000/rate);

	}

/*
	for (int i=0; i < 20; i++) {
		msg_spheres_id msg = {0};

		retVal = write(serial_fd, (void *)&requestByte, sizeof(requestByte));
		if (retVal < sizeof(requestByte))
		{
			printf("Write failed\n");
			exit(0);
		}

		int totalBytes = 0;

		while (totalBytes < sizeof(msg)) {
			retVal = read(serial_fd, ((unsigned char *) &msg)+totalBytes, sizeof(msg)-totalBytes);
			if (retVal == -1)
			{
				printf("read() 1 failed: %s\n", strerror(errno));
				exit(0);
			}
			totalBytes+=retVal;
		}

		if (msg.messageID != 0xA3A3A302 || msg.programID != 3315 || msg.spheresHWID != 54) {
			errorCount++;
		}
		usleep(100000);
	}

	printf("Error Count: %d\n", errorCount);
*/
	closeSpheres();
}
