#ifndef _HALO_INTERFACE_H_
#define _HALO_INTERFACE_H_

// include all class headers here
#include "serialib.h"
#include "FTDI.h"

// C++ standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <sys/resource.h>

//define bytes to be written
#define ALL_PORTS_ON    0x97 
#define ALL_PORTS_OFF   0xA7  

#define HALO_SERIAL_FILE "../HaloCore/GSP/HALO_SERIAL_FILE"

using namespace std;

class Halo {

	friend class haloGSP; // GSP can access and call private functions, but derived GS class cannot directly call private functions with SPHERES

	serialib HPIC;
	FTDI ftdi;
	string pathToPIC;

public:
	bool useHalo;
	bool allPortsOn;
	bool allPortsOff;
	//declare serial ports
	unsigned char HP1;
	unsigned char HP2;
	unsigned char HP3;
	unsigned char HP4;
	unsigned char HP5;
	unsigned char HP6;
	Halo () 
	{
		//default settings
		useHalo = true;
		allPortsOn = false;
		allPortsOff = false;	
	}

	void findPathToPIC(); //modifies the pathToPIC string to contain /dev/ttyUSBx
	int parseHaloSerialFile();

    /*The following four methods are not implemented, but may be implemented in the future.
    void powerOnPort(int port_number);
    void powerOffPort(int port_number);
    void sendDataToPort(int port_number, GSdataStructure data); //send data to specific port
	GSdataStructure getDataFromPort(int port_number); //get data from specific port
	*/

private:
	unsigned int initHalo();
	unsigned int closeHalo();
};

#endif
