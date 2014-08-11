#include "halo.h"

unsigned int Halo::initHalo()
{
	int retVal;

	findPathToPIC(); //sets pathToPIC string to path to any connected device that matches a Halo serial number

	//open serial comm port
	retVal = HPIC.Open(pathToPIC.c_str(), 19200);
	switch(retVal)
	{
	case 1: cout << "Comm port opened successfully!" << endl; break;
	case -1: cout << "Device not found" << endl; break;
	case -2: cout << "Error while opening the device" << endl; break;
	case -3: cout << "Error while getting port parameters" << endl; break;
	case -4: cout << "Speed (Bauds) not recognized" << endl; break;
	case -5: cout << "Error while writing port parameters" << endl; break;
	case -6: cout << "Error while writing timeout parameters" << endl; break;
	default: cout << "unknown error" << endl;
	}

	//write default settings (all ports off) to PIC
	cout << "Writing default settings (ALL_PORTS_OFF)" << endl;
	retVal = HPIC.WriteChar(ALL_PORTS_OFF);
	switch(retVal)
	{
	case 1: cout << "Written successfully!" << endl; break;
	case -1: cout << "Error while writing data" << endl; break;
	default: cout << "unknown error" << endl;
	}

	//write to turn all ports on if allPortsOn flag is used
	if(allPortsOn)
	{
		cout << "Writing to turn all Halo Ports on" << endl;
		retVal = HPIC.WriteChar(ALL_PORTS_ON);
		switch(retVal)
		{
			case 1: cout << "Written successfully!" << endl; break;
			case -1: cout << "Error while writing data" << endl; break;
			default: cout << "unknown error" << endl;
		}
		goto allPortsUsed; //skips writing to other ports if all ports are already on
	}

	//skips writing to ports if allPortsOff flag is used
	if(allPortsOff)
	{
		cout << "All Halo Ports turned off!" << endl;
		goto allPortsUsed; //skips writing to other ports if all ports are already off and sticks with default settings
	}

	//write to serial comm port for each Halo port, which will set it on/off depending on what parameter file dictates
	cout << "Writing to Port - HP1" << endl;
	retVal = HPIC.WriteChar(this->HP1);
	switch(retVal)
	{
	case 1: cout << "Written successfully!" << endl; break;
	case -1: cout << "Error while writing data" << endl; break;
	default: cout << "unknown error" << endl;
	}
	cout << "Writing to Port - HP2" << endl;
	retVal = HPIC.WriteChar(this->HP2);
	switch(retVal)
	{
	case 1: cout << "Written successfully!" << endl; break;
	case -1: cout << "Error while writing data" << endl; break;
	default: cout << "unknown error" << endl;
	}
	cout << "Writing to Port - HP3" << endl;
	retVal = HPIC.WriteChar(this->HP3);
	switch(retVal)
	{
	case 1: cout << "Written successfully!" << endl; break;
	case -1: cout << "Error while writing data" << endl; break;
	default: cout << "unknown error" << endl;
	}
	cout << "Writing to Port - HP4" << endl;
	retVal = HPIC.WriteChar(this->HP4);
	switch(retVal)
	{
	case 1: cout << "Written successfully!" << endl; break;
	case -1: cout << "Error while writing data" << endl; break;
	default: cout << "unknown error" << endl;
	}
	cout << "Writing to Port - HP5" << endl;
	retVal = HPIC.WriteChar(this->HP5);
	switch(retVal)
	{
	case 1: cout << "Written successfully!" << endl; break;
	case -1: cout << "Error while writing data" << endl; break;
	default: cout << "unknown error" << endl;
	}
	cout << "Writing to Port - HP6" << endl;
	retVal = HPIC.WriteChar(this->HP6);
	switch(retVal)
	{
	case 1: cout << "Written successfully!" << endl; break;
	case -1: cout << "Error while writing data" << endl; break;
	default: cout << "unknown error" << endl;
	}

	allPortsUsed:

	return 0;
}

void Halo::findPathToPIC() {
	string originalWD;
	originalWD = this->ftdi.getCWD(); //saves current working directory to be reset later

	int retVal = parseHaloSerialFile();
	switch (retVal) {
	case 1:
		cout << "Found Halo Serial Path: " << pathToPIC << endl;
		break;
	case -1:
		cout << "Could not find any matching serial numbers." << endl;
		break;
	case -2:
		cout << "Could not open HaloSerialFile." << endl;
		break;
	default:
		cout << "Unknown error when trying to match Halo serial number to connected devices." << endl;
		break;
	}

	ftdi.changeWD(originalWD); //reset working directory to original working directory
}

int Halo::parseHaloSerialFile() {
	string line;
	string HaloID;
	ftdi.run(); //get the list of all the matching ones

	ifstream HaloSerialFile(HALO_SERIAL_FILE); //open the HALO_SERIAL_FILE, defined in header
	if (HaloSerialFile.is_open()) {
		for (int count = 0; count < 20; count ++) { //getline 20 times (in future if file gets longer, just increase count bound)
			getline(HaloSerialFile, line);
			if (line.substr(0, 1) != "#") {
				HaloID = ftdi.checkSerial(line);
				if (HaloID != "") { //if the found USB name is not empty
					pathToPIC = "/dev/" + HaloID;
					return 1;
				}
			}
		}
		return -1; //return -1 if none of connected devices worked
	}
	else
		return -2; //return -2 if HaloSerialFile is not open
}

unsigned int Halo::closeHalo()
{
	//close serial port
	HPIC.Close();

	printf("Closed halo!\n");
	return 0;
}
