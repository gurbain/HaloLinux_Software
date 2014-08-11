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

#include "spheres.h"

using namespace std;

#define SERIAL_PORT	"/dev/ttyS0"		//ttyS0 for pico-itx board, ttyUSB0 for keyspan usb

/*#define START_BYTE 			0xA7
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
*/
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

int sendUserMessage(unsigned char user_message_id, void * msgData, unsigned char msgLength) {
	int retVal;
	msg_header msg;

	msg.startByte = START_BYTE;
	msg.from = FROM_SPHERES;
	msg.length = msgLength;
	msg.ID = user_message_id;

	printf("Sending User Message: %X\n", user_message_id);
	retVal = write(serial_fd, (void *)&msg, sizeof(msg));
	if (retVal < sizeof(msg))
	{
		printf("Write Header failed: %d\n", retVal);
		return -1;
	}

	if (msgData != NULL) {
		retVal = write(serial_fd, msgData, msgLength);
		if (retVal < sizeof(msgData))
		{
			printf("Write failed\n");
			return -1;
		}
	} else if (msgLength != 0) {
		printf("No data provided\n");
		return -1;
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
		printf("Open Serial Port Failed: %s\n", serialport.str().c_str());
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
	options.c_iflag &= ~(IGNBRK | BRKINT | ICRNL | INLCR | PARMRK | INPCK | ISTRIP | IXON);

	//set canonical
//	options.c_lflag = ICANON;
//	options.c_lflag &= ~ICANON;		//old
	options.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG); //new

	options.c_oflag &= ~(OCRNL | ONLCR | ONLRET | ONOCR | OFILL | OLCUC | OPOST);

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
	cout << "Running SerialSendTest\n\n";


	unsigned char msg[256];
	unsigned char messagelength;
	msg_header head;
	int totalBytes;

	msg_imu_data imu;
	msg_body_forces_torques forceTorque;
	msg_state globMet;

	//usleep(6000000);

	sendUserMessage(SYNC_TEST, NULL, 0);

	usleep(1000000);

	imu.accel[0] = 1.4;
	imu.accel[1] = 2.5;
	imu.accel[2] = 3.6;
	imu.gyro[0] = 4.1;
	imu.gyro[1] = 5.2;
	imu.gyro[2] = 6.3;

	forceTorque.forces[0] = 11.1;
	forceTorque.forces[1] = 22.2;
	forceTorque.forces[2] = 33.3;
	forceTorque.torques[0] = 111.1;
	forceTorque.torques[1] = 122.2;
	forceTorque.torques[2] = 133.3;

	globMet.pos[0] = 1000;
	globMet.pos[1] = 1001;
	globMet.pos[2] = 1002;
	globMet.vel[0] = 1010;
	globMet.vel[1] = 1011;
	globMet.vel[2] = 1012;
	globMet.quat[0] = 1020;
	globMet.quat[1] = 1021;
	globMet.quat[2] = 1022;
	globMet.quat[3] = 1023;
	globMet.angvel[0] = 1030;
	globMet.angvel[1] = 1031;
	globMet.angvel[2] = 1032;

	sendUserMessage(IMU_DATA, &imu, sizeof(imu));

	int count = 0;

	while (count++ < 10000) {
		imu.testTime = count * 100 +11;
		forceTorque.testTime = count * 1000 + 22;
		globMet.testTime = count * 10 +3;
		sendUserMessage(IMU_DATA, &imu, sizeof(imu));
		sendUserMessage(ACT_FORCES_TORQUES, &forceTorque, sizeof(forceTorque));
		sendUserMessage(GLOBAL_MET, &globMet, sizeof(globMet));
		usleep(500);
	}


	closeSpheres();
}
