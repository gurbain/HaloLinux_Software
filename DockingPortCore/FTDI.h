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
	string serialNumbers[10];
	string ttyNames[10]; 
	string ttyName;

public:
	FTDI(){};
	void emulateTerminalCommands(string const& dir1, string const& dir2); //emulates the terminal commands to cd into the link of the ttyUSB(#) path, then cd up four directories, then try opening the serial file
	string getCWD(); //gets the current working directory
	void changeWD(string const& changePath); //changes the current working directory to that of changePath
	void storeSerialID(string const& serial); //stores serial of device to serialNumbers array and ttyName of serial to ttyNames array (same index)
	string getSerialID(int newIndex); //returns serial (from serialNumbers array) at index newIndex
	string getName(int newIndex); //returns name (from ttyNames array) at index newIndex
	int openSerialFile(); //tries opening the serial file that should exist if a device is connected
	int getNumberofConnectedDevices(); //returns 'index', which is updated each time a connected device is found to represent total connected devices
	void setupCommList(); //sets up list of all comm ports found in /sys/class/tty and then calls emulateTerminalCommands
	string checkSerial(string const& serial); //used in peripherals to check serial of peripheral with list of used serial IDs, returns ttyName of found serialID
	void run(); //runs entire class
};	

#endif
