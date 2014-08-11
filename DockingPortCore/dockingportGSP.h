/*!
 *  \brief     VERTIGO Guest Scientist Program.
 *  \details   This is the primary GSP class for VERTIGO.
 *  \author    Konrad Makowka
 *  \author    Brent Tweddle
 *  \version   4.1a
 *  \date      2011-2012
 *  \copyright TBD
 */

#ifndef _DOCKINGPORTGSP_H_
#define _DOCKINGPORTGSP_H_

class dockingportGSP;

// include all class headers here
#include "dockingport_Camera.h"
#include "dockingport_DataStorage.h"
#include "dockingport_Rectifier.h"
#include "serialib.h"
#include "FTDI.h"
#include "../HaloCore/GSP/networking.h"
#include "../HaloCore/GSP/videoStreaming.h"
#include "../HaloCore/GSP/haloGSP.h"

// OpenCV
#include "opencv2/core/core.hpp"

// C++ standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <uEye.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <vector>
#include <ctime>
#include <cerrno>
#include <deque>

#define UDP_SERIAL_FILE "../DockingPortCore/UDP_SERIAL_FILE"

using namespace std;

class timestampedsingleImage { //class with a timestamp and a Mat image
	//storeimg will be stored into the Results file
private:
	cv::Mat storeimg, singlestoreimg;
	double timestamp;
public:
	timestampedsingleImage(double _timestamp, cv::Mat& singleImage) {
			timestamp = _timestamp;
			storeimg.create(cv::Size(640,480), CV_8UC3); //create storeimg with correct dimensions
								     //CV_8UC3 indicates greyscale, no color
			singlestoreimg = storeimg(cv::Rect(0,0,640,480)); //let singlestoreimg be the part of storeimg that encompasses those dimensions
									  //necessary so that you can modify it
			singleImage.copyTo(singlestoreimg); //take input singleImage and copy it to singlestoreimg, which in turn modifies storeimg
	}

	void writeImg(dockingport_DataStorage& ds) { //saving storeimg under the correct name to the correct directory
		ds.timetagStorageFile << ds.numsaved << "," << timestamp << "\n";
		sprintf(ds.singleImageStorageName, "%s/SingleImage%d.bmp", ds.imagestoragepath, ds.numsaved);
		cv::imwrite(ds.singleImageStorageName, storeimg);
		ds.numsaved++;
	}

	void deallocImage() {
		storeimg.deallocate();
	}

	double getTimestamp() {
		return timestamp;
	}

};

class dockingportGSP : virtual public haloGSP{ 

protected:
	// for timestamps
	double imageTimeStamp;
	double prevImageTimeStamp;

private:
//	double zerotime;
//	timeval timeRAW;

	HIDS camera_ID; //used in datastorage

	struct timespec testStartTime, currentTime;

	// Mutexes
	pthread_mutex_t storageDeque_mutex;

	// Image Storage Thread
    void imageStorage_thread();
	static void * imageStorage_thread_Helper(void * This)
	{
		((dockingportGSP *)This)->imageStorage_thread();
		return NULL;
	};

public:

//Moved to public from private
	pthread_t ImageStorageThread;
	std::deque<timestampedsingleImage> storageImg_deque;
//end moved to public from private

	//test time in millisecond
	double testtime;

	bool useUDP_PIC;

	bool newImagesAvailable;
	bool stopImageStorageThread;
	int synchCheckFlag;
	
	char calibParamSetName[200]; // name of camera calibration set to be used
	char calibParamDir[200];
	char dockportName[200];

	//used in parseParameterFile
	char UDP_ID[100][11]; //stores all UDP IDs found in UDP Serial File
	char UDP_storage_ID[11]; //stores ID from constructor
	char FTDI_serID[100][11];
	bool foundUDPID; //used to check UDP_ID from TP with paramFile in parseUDPSerialFile

	char* Buffer_packet; //used for reading chars from PIC
	unsigned char pic_byte; //returned by readPIC() method

	cv::Mat singleImage;//, ImageCopy;
	
	dockingport_Camera camera; //camera ID will be passed in dockingportGSP.cpp in camera initialization section
	dockingport_Rectifier rectifier;
	dockingport_DataStorage datastorage;
	
	//PIC
	serialib PIC; 

	//FTDI
	FTDI ftdi; 
	string serPath;	
	string FTDI_ID;

	//variables for reading image from file
	string camInputImgDir;
	int camInputStartImg;
	int camInputFinalImg;
	int camInputImgCounter;

	dockingportGSP(haloGSP *halo, char _UDP_ID[11]) {

        //gettimeofday(&test,NULL);
        //zerotime = timeRAW.tv_sec*1000+timeRAW.tv_usec/1000; // Time when program was started in ms; zerotime calculated again when main program loop is entered
 		
		//Parse the UDP Serial File
		sprintf(UDP_storage_ID, "%s", _UDP_ID);
		foundUDPID = false;
		this->parseUDPSerialFile();

		//Set correct parameter paths
		sprintf(this->GPFpath, "%s", halo->GPFpath);
		sprintf(this->calibParamSetName, "%s", halo->calibParamSetName);
		sprintf(this->parameterFilePath, "%s", halo->parameterFilePath);

		//Datastorage Labels
		sprintf(this->testname, "%s", halo->testname); //testname
		sprintf(this->dockportName, "UDP"); //name of peripheral
		sprintf(this->runPath, "%s", halo->runPath); //runPath to Results/TP/run set by halo datastorage

		camInputStartImg = 0;
		camInputFinalImg = -1;
		camInputImgCounter = 0;

		// internal flag init settings
		newImagesAvailable = false;
		stopImageStorageThread = false;
		synchCheckFlag = 0; // 0 means, that frames in current timestep are in synch
				    // -1 if out of synch
				    // this flag is only changed if camera.useSynchCams==true, since notSynch camera are out of synch anyway

		if (calibParamSetName[0] == '\0') { //check if string is empty, i.e. the first character is the null terminating character
			sprintf(calibParamSetName, "defaultSet"); // standard camera calibration set
		};
	}

	//Moved to public from private
	void shutdown();
	void captureAndRectifySingleImage(cv::Mat& singleImg); //main function responsible for capturing images
	void connectToPIC(); //connect to right FTDI port using serial number
	void init();
	void parseParameterFile(string line, bool printParams); //set printParams true for first peripheral used, false if more used
	void parseUDPSerialFile(); //parses the UDP Serial File to set the right camera IDs and FTDI IDs based on input UDP ID

	//methods that write to the UDP PIC
	void motorForward();
	void motorReverse();
	void motorOff();
	unsigned char readPIC(); //prompts PIC for data then returns the byte that PIC sends back; used to monitor voltage

};

#endif
