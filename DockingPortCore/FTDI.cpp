#include "FTDI.h"
    
string FTDI::getCWD()
{
    string CurrentPath; 
    char buffer[128];
    char *path = getcwd(buffer, sizeof(buffer)); //gets current directory

    if (!path) 
    {
    	perror("bad path to current directory"); //check if directory is good
    } 
    else 
    {
        CurrentPath = path;
		return CurrentPath; //returns the current working directory, a string
    }
}

void FTDI::changeWD(string const& changePath)
{
    int retVal;	
    char buffer[128];
    retVal = chdir(changePath.c_str()); //changes directory
    if (retVal < 0) 
    {
		char *errorMessage = strerror_r(errno, buffer, sizeof(buffer));
		string error = errorMessage;
		cout << "error is: " << error << endl; //displays error message chdir fails
    }
}

int FTDI::openSerialFile()
{
    ifstream serialFile;
    char serial[16];
    serialFile.open("serial"); //tries opening the 'serial' file 
    if(serialFile.is_open())
    {
    	serialFile >> serial;
    	storeSerialID(serial); //stores the associated serial number only if the serial file exists
		serialFile.close();
    	return 1; //indicates serial number found
    }
    serialFile.close();
    return -1; //indicates serial number not found
}

void FTDI::emulateTerminalCommands(string const& dir1, string const& dir2) {
   
    int retVal;
	string fullPath;

    //cout<< "before changing directories" << endl;
    changeWD(dir1); //change directory to dir1
    fullPath = getCWD(); //saves the full path to the connected device '/dev/.../ttyUSBx/tty/ttyUSBx'

	//updates the ttyName from the fullPath
    unsigned found = fullPath.find_last_of("/\\"); //finds last instance of '/' in the full path
    ttyName = fullPath.substr(found+1); //only stores end of the fullPath to ttyName

    //cout << "after changing to dir1" << endl;
    changeWD(dir2); //change directory to dir2
    //cout << "after changing to dir2" << endl << endl;

	//open serial file
    retVal = openSerialFile(); // Tries opening serial file, and if it exists, it saves the existing serial number to serialNumbers 
    switch (retVal)
    {
		case -1: 
			break;
		case 1:
			//cout << "found serial port in use!" << endl;
			break;
		default:
			break;
    }
}

void FTDI::setupCommList() 
{
    int n;
    struct dirent **namelist; //use to find name of link to USB ports
    const char* ttyDir = "/sys/class/tty/";
    const char* fourDir = "../../../..";
    string fDir = fourDir;

    // Scan through /sys/class/tty - it contains all tty-devices in the system
    n = scandir(ttyDir, &namelist, NULL, NULL);
    if (n < 0)
        perror("scandir");
    else 
    {
        while (n-->0) 
		{
            if (strcmp(namelist[n]->d_name,"..") && strcmp(namelist[n]->d_name,".")) 
	        {
                // Construct file path
                string devicedir = ttyDir;
                devicedir += namelist[n]->d_name; //d_name contains location of where ttyUSBx link points to
				emulateTerminalCommands(devicedir, fDir); //changes the working directory to four directories above devicedir and tries opening serial file
            }
            free(namelist[n]); //deletes allocated memory to array
        }
        free(namelist);
    }  
}

void FTDI::run(){
    
    string serNo;
    string name;
    this->setupCommList(); //sets up the list of connected FTDI IDs and their respective 'ttyUSB(#)' names and then emulate terminal commands to save appropriate paths
    int totalEntries = this->getNumberofConnectedDevices();
    for(int i=0; i<totalEntries; i++)
    {
		serNo = this->getSerialID(i);
		name = this->getName(i);
		cout << "Serial number: " << serNo << " and ttyName: " << name << endl << endl;
    } 
}

void FTDI::storeSerialID(string const& serial)
{
    serialNumbers[index] = serial;
    ttyNames[index] = this->ttyName;
    index++;
}

string FTDI::getSerialID(int newIndex)
{
	return this->serialNumbers[newIndex];
}

string FTDI::getName(int newIndex)
{
	return this->ttyNames[newIndex];
}

int FTDI::getNumberofConnectedDevices()
{
	return this->index;
}

string FTDI::checkSerial(string const& serial)
{
    for(int i=0; i<index; i++)
    {
		if(serialNumbers[i].compare(serial) == 0)
		{
			 return ttyNames[i];
		}
		else
		{
			 return NULL;
		}
   }
}
