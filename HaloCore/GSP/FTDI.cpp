#include "FTDI.h"

string FTDI::checkSerial(const string &serial) { //called in Halo class to connect to the Halo PIC
	for (int i = 0; i < index; i ++) {
		if (serialNumbers[i].compare(serial) == 0)
			return ttyNames[i]; //if the number matches, return the corresponding name
		return NULL;
	}
}

void FTDI::run() { //creates array of serial numbers and array of names, and prints them
	string serNo;
	string name;
	setupCommList();
	int totalEntries = getNumberofConnectedDevices();
	for (int i = 0; i < totalEntries; i++) {
		serNo = getSerialID(i);
		name = getName(i);
		cout << "Serial Number: " << serNo << " and ttyName: " << name << endl;
	}
}

void FTDI::setupCommList() {
	int n;
	struct dirent **namelist; //used to find names of links to USB ports
	const char *ttyDir = "sys/class/tty/";
	const char *fourDir = "../../../..";
	string fDir = fourDir;

	//scan through the /sys/class/tty directory
		//scandir(const char* dir, struct dirent*** namelist)
		//stores names of all files and directories in dir in an array called namelist
		//returns an integer representing number of items returned
	n = scandir(ttyDir, &namelist, NULL, NULL);
	if (n < 0)
		perror("Error in scandir()");
	else {
		while (n-- > 0) {
			//d_name contains location of where ttyUSBx link points to
			//if the location is
			if (strcmp(namelist[n]->d_name, "..") && strcmp(namelist[n]->d_name, ".")) {
				//construct file path
				string devicedir = ttyDir;
				devicedir += namelist[n]->d_name;
				emulateTerminalCommands(devicedir, fDir); //tries to open serial file located four directories above devicedir
			}
			free(namelist[n]); //deletes allocated memory
			}
		free(namelist);
		}
	}

void FTDI::emulateTerminalCommands(const string &dir1, const string &dir2) {
	string fullPath;
	changeWD(dir1);
	fullPath = getCWD(); //gets full path to connected device, "dev/.../tty/USBx/tty/ttyUSBx"

	//updates ttyName to the correct name, which is the last part of the fullPath
	unsigned found = fullPath.find_last_of("/");
	ttyName = fullPath.substr(found+1, string::npos); //gets text from the last '/' character to the end

	changeWD(dir2);

	//tries to open serial file; if the file exists, save found serial number to serialNumbers
	retVal = openSerialFile();
	switch(retVal) {
	case 1: //successfully found a port
		//cout << "Found serial port that's in use!" << endl;
		break;
	default:
		break;
	}
}

int FTDI::openSerialFile() {
	ifstream serialFile;
	char serial[16];
	serialFile.open("serial");
	if (serialFile.is_open()) {
		serialFile >> serial; //stream content of serialFile
		storeSerialIDAndName(serial); //stores the associated serial number if the serial file at the location exists
		serialFile.close();
		return 1; //indicates success
	}
	serialFile.close();
	return -1;
}

void FTDI::storeSerialIDAndName(const string &serial) {
	serialNumbers[index] = serial;
	ttyNames[index] = ttyName; //remember ttyName was set in emulateTerminalCommands
	index ++;
}

string FTDI::getSerialID(int newIndex) {
	return serialNumbers[newIndex];
}

string FTDI::getName(int newIndex) {
	return ttyNames[newIndex];
}

int FTDI::getNumberofConnectedDevices() {
	return this->index; //at any given moment the number of connected devices is the current value of index
}

void FTDI::changeWD(const string &changePath) {
	char buffer[128];
	retVal = chdir(changePath.c_str()); //changes to that directory and returns 1 if successful
	if (retVal < 0) {
		char *errorMessage = strerror_r(errno, buffer, sizeof(buffer)); //strerror_r maps the error number to an error message string and returns a pointer to it
		string error = errorMessage;
		cout << "error is: " << error << endl;
	}
}

string FTDI::getCWD() {
	string CurrentPath;
	char buffer[128];
	char *path = getcwd(buffer, sizeof(buffer)); //getcwd puts absolute pathname into array pointed by buffer, then return buffer

	if (!path) //if path is null (returns false)
		perror("Bad path to current directory");
	else {
		CurrentPath = path;
		return CurrentPath; //returns a string representing current path
	}
}







