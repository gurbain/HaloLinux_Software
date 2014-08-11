/*!
 *  \brief     Guest Scientist Program for tests involving the Halo.
 *  \details   This is the primary GSP class for tests involving the Halo.
 *  \author    Bryan McCarthy
 *  \author    Chris Jewison
 *  \version   0.1
 *  \date      Feb 2014
 *  \copyright TBD
 */

#ifndef _HALO_GSP_H_
#define _HALO_GSP_H_

class haloGSP;

// include all class headers here
#include "spheres.h"
#include "halo.h"
#include "videoStreaming.h"
#include "networking.h"
#include "halo_DataStorage.h"

// C++ standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
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

using namespace std;

#define TEST_PROJ_CHANNEL_FILE "/opt/GogglesDaemon/TEST_PROJ_CHANNEL"
#define CAMERA_FILE "/opt/GogglesOptics/CAMERA_FILE" //HL


class haloGSP
{
/////////////////////////// Public Main Functions
public:
	//constructor
	haloGSP() {
		// internal flag init settings
		shutdownCriterion = false;

		pthread_mutex_init(&VideoStreamingMutex, NULL);
	}

	// Signal Handling, if Program shall be terminated (Either by IP Comm Block or CTRL+C)
	// This Function is not a class member on purpose
	void terminateHandler(int sig)
	{
		cout << endl << endl << "Program terminated with Signal: " << sig << endl;
		this->shutdownCriterion = true;

	}
	// starting a Linux process and returning "system" output
	string execAndReturnSystemOutput(char* cmd);
	void runMain(int argc, char *argv[]);

private:

	struct timespec testStartTime, currentTime;

//*************** Private Main Functions *****************

	//Core Functionalities:
	void init();
	void initBackgroundTask();
	void shutdown();

    // Parsing
    void parseCommandlineArgs(int argc, char *argv[]);
    void parseParameterFile();

//************ End Main Functions ****************

//******************** Threads *******************
	// P Threads
	pthread_t SpheresThread;
	pthread_t HaloThread; //RS added
	pthread_t ImuProcessingThread;
	pthread_t ForceTorqueProcessingThread;
	pthread_t GlobMetProcessingThread;
	pthread_t BackgroundTaskThread;
	pthread_t VideoStreamingThread;

	// Mutexes
	pthread_mutex_t storageDeque_mutex;
	pthread_mutex_t VideoStreamingMutex;
	//std::deque<timestampedImage> storageImg_deque;

    // Spheres Thread
	void spheres_thread();
    static void * spheres_thread_Helper(void * This)
    {
    	((haloGSP *)This)->spheres_thread();
    	return NULL;
    }

    //Halo Thread
    void halo_thread(); //RS added
    static void * halo_thread_Helper(void * This)
	{
		((haloGSP *)This)->halo_thread();
		return NULL;
	}

    // Video Streaming Thread
    void videoStreaming_thread();
    static void * videoStreaming_thread_Helper(void * This)
    {
        ((haloGSP *)This)->videoStreaming_thread();
        return NULL;
    }

    // IMU Processing Thread
	void imuProcessing_thread();
    static void * imuProcessing_thread_Helper(void * This)
    {
    	((haloGSP *)This)->imuProcessing_thread();
    	return NULL;
    }

    // Forces Torques Processing Thread
	void forceTorqueProcessing_thread();
    static void * forceTorqueProcessing_thread_Helper(void * This)
    {
    	((haloGSP *)This)->forceTorqueProcessing_thread();
    	return NULL;
    }

    // Forces Torques Processing Thread
	void globMetProcessing_thread();
    static void * globMetProcessing_thread_Helper(void * This)
    {
    	((haloGSP *)This)->globMetProcessing_thread();
    	return NULL;
    }

    // Background Task Thread
	void backgroundTask_thread();
    static void * backgroundTask_thread_Helper(void * This)
    {
    	((haloGSP *)This)->backgroundTask_thread();
    	return NULL;
    }

// ******************End Threads ********************

public:

//****** Declare Miscellaneous Variables ************

	//test time in millisecond
	double testtime;

	// GSP Flags
	bool shutdownCriterion; // while loop executed until this is true or test ended by SPHERES
	int GSPretVal;
	bool useBackgroundTask;

	// Variables to store paths
	char GPFpath[200]; // path to GPF folder, read in from command line
	char calibParamSetName[200]; //set so that TP objects have this as a member variable to access
	char testname[100];  //read in from command line by looking at the executable name
	char runPath[200]; //path to run folder, set in Halo DataStorage

	char parameterFilePath[200];
	char parameterFileDir[200];
	char parameterFileName[100];


	// Videostreaming
	string currentVideoStreamingModeName;
	string GSvideoStreamingModeFrameNames;

	VideoStreaming videostreaming;
	Spheres spheres;
	Halo halo; // RS added
	halo_DataStorage halodatastorage;

	// declare default videoBuffer
	MatVideoBuffer defaultVideoBuffer;

	//// Supplementary Functionalities
	double updateTime();

	// set variables for Videostreaming buffer

	//updates the testtime variable
	double timeDiff(struct timespec *start, struct timespec *end);

//*********** End Miscellaneous Variables **************

	
//******************** GS FUNCTIONS *********************
	virtual void GSinit();

	virtual void initVideoStreaming(); // called after GSsetup, since GS input needed first

	virtual void GScustomThreads();

	virtual void GSsetup();

	virtual void GSbackgroundTask(); //background processing running at 10ms loop (if enabled with useBackgroundTask)

	virtual void GSprocessIMU(double timestamp, msg_imu_data imu_data); // IMU data processing science algorithms

	virtual void GSprocessForceTorque(double timestamp, msg_body_forces_torques forceTorque_data); // Force and torque (commanded) data processing science algorithms

	virtual void GSprocessGlobalMetrology(double timestamp, msg_state globMet_data); // Global metrology (13 element state vector) data processing science algorithms

	virtual void GSrunMain(); // Main loop for Guest Scientist code

	virtual void GScleanup(); // potential cleanup function that can be called after test is finished to store/delete data

	virtual	void GSparseCommandlineArgs(int argc,char *argv[]);

	virtual void GSparseParameterFile(string line);

	virtual void GSprocessGuiMouseClick(int x, int y);		//process a mouseclick from the GUI

	virtual void GSprocessGuiKeyPress(unsigned char key);


}; //end class definition

#endif
