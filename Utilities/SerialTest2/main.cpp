#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cstdio>
#include <sstream>
#include <termios.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>

using namespace std;

#define SERIAL_PORT	"/dev/ttyUSB0"		//ttyS0 for pico-itx board, ttyUSB0 for keyspan usb

#define START_BYTE 			0xA7
#define FROM_SPHERES		0x01
#define FROM_GOGGLES		0x02
#define WATCHDOG_REQUEST	0xB1
#define WATCHDOG_REPLY		0xB2

typedef struct {
	unsigned char startByte;
	unsigned char length;
	unsigned char from;
	unsigned char ID;
} msg_header;

int serial_fd;
istringstream serialport;


double timeDiff(struct timespec *start, struct timespec *end)
{
	double milliseconds;
	double tv_sec, tv_nsec;
	if ((end->tv_nsec-start->tv_nsec)<0) {
		tv_sec = end->tv_sec-start->tv_sec-1;
		tv_nsec = 1.0e9+end->tv_nsec-start->tv_nsec;
	} else {
		tv_sec = end->tv_sec-start->tv_sec;
		tv_nsec = end->tv_nsec-start->tv_nsec;
	}
	milliseconds = tv_sec*1.0e3 + (tv_nsec / 1.0e6);
	return milliseconds;
}

double round (double value)
{

	if (value < 0)
	{
		return -(floor(-value+0.5));
	}
	else
	{
		return floor(value+0.5);
	}
}


unsigned int initSpheres()
{
	int retVal;
	struct termios options;

	//open the serial port
	serial_fd = open(serialport.str().c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (serial_fd == -1)
	{
		printf("Open Serial Port Failed: %s\n", SERIAL_PORT);
		exit(1);
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


struct timespec time1, time2;
double delT;

int main (int argc, char *argv[])
{
	int retVal;
	unsigned char requestByte = 0xA1;
	unsigned char retByte;
	int errorCount = 0;

	clock_gettime(CLOCK_REALTIME, &time1);

	if (argc != 2) {
		serialport.str(SERIAL_PORT);
		std::cout << "No Serial Port Provided" << std::endl;
	} else {
		serialport.str(argv[1]);
	}
	std::cout << "Using Serial Port: " << serialport.str() << std::endl;

	initSpheres();
	cout << "Running SerialTest2\n\n";


	unsigned char msg[256];
	unsigned char messagelength;
	msg_header msgHeader;
	int totalBytes;

	while(1)
	{

		totalBytes = 0;
		while (totalBytes == 0) {
			retVal = read(serial_fd, (unsigned char*)&msgHeader, 1);
			if (retVal == -1)
			{
				printf("read() message failed: %s\n", strerror(errno));
			}
			if (msgHeader.startByte != START_BYTE) {
				printf("Bad start byte on read()\n");
				totalBytes = 0;
			} else {
				totalBytes+=retVal;
			}
		}

		while (totalBytes < sizeof(msgHeader)) {
			retVal = read(serial_fd, (unsigned char*)&msgHeader+totalBytes, sizeof(msgHeader)-totalBytes);
			if (retVal == -1)
			{
				printf("read() message failed: %s\n", strerror(errno));
			}
			totalBytes+=retVal;
		}
		clock_gettime(CLOCK_REALTIME, &time2);
		delT = round(timeDiff(&time1, &time2))/1000;

		printf("[%.3f] MSG RCV: StartByte=0x%X, Length=0x%X, From=0x%X, ID=0x%X\n", delT, msgHeader.startByte, msgHeader.length, msgHeader.from, msgHeader.ID);

		if (msgHeader.startByte != START_BYTE || msgHeader.from != FROM_SPHERES) {
			printf("Bad start byte on message\n");
			continue;
		}

		if ( msgHeader.ID == WATCHDOG_REQUEST) {
			msg_header msg;
			msg.startByte = START_BYTE;
			msg.ID = WATCHDOG_REPLY;
			msg.from = FROM_GOGGLES;
			msg.length = 0;

			retVal = write(serial_fd, (void *)&msg, sizeof(msg));
			if (retVal < sizeof(msg))
			{
				printf("Write failed\n");
				return -1;
			}
			clock_gettime(CLOCK_REALTIME, &time2);
			delT = round(timeDiff(&time1, &time2))/1000;

			printf("[%.3f] MSG REPLY: StartByte=0x%X, Length=0x%X, From=0x%X, ID=0x%X\n\n", delT, msg.startByte, msg.length, msg.from, msg.ID);
		} else {
			printf("Bad Message ID\n");
		}

		usleep(100);
	}

	closeSpheres();
}
