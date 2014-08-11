#ifndef _FTDI_H_
#define _FTDI_H_

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#include <errno.h>

using namespace std;

class FTDI {
	int index;
	int retVal;
	string serialNumbers[10];
	string ttyNames[10];
	string ttyName;

public:
	FTDI() {};

	//emulate terminal commands to cd into link of the ttyUSB path, then cd up 4 directories, then open serial file
	void emulateTerminalCommands(const string &dir1, const string &dir2);

	//gets current working directory
	string getCWD();

	//changes current working directory to changePath
	void changeWD(const string &changePath);

	//store serial device to serialNumbers array, and ttyName of serial to ttyNames array at the same index
	void storeSerialIDAndName(const string &serial);

	//return the serial number at index newIndex from serialNumbers array
	string getSerialID(int newIndex);

	//returns name at index newIndex from ttyNames array
	string getName(int newIndex);

	//open the serial file that should exist if the device is connected
	int openSerialFile();

	//returns the class variable inex, which is updated each time a connected device is found
	int getNumberofConnectedDevices();

	//sets up list of all the comm ports found in /sys/class/tty then calls emulateTerminalCommands on each
	void setupCommList();

	//compares argument serial to array of serialNumbers and finds a match, returns the corresponding ttyName
	string checkSerial(const string &serial); //called in the peripheralGSP classes

	//main method
	void run();
};

#endif
