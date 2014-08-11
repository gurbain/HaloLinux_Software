#ifndef _TESTPROJECT_H_
#define _TESTPROJECT_H_

#include "../../DockingPortCore/dockingportGSP.h"
#include "../../OpticsMountCore/opticsmountGSP.h"
//add any other necessary header files here

using namespace cv;

//do not change the class inheritance, it should derive from HaloGSP
class testproject: virtual public haloGSP {

private:
    int maneuverNumber; 
	pthread_mutex_t keymutex;
	double averageTime;
	int time_samples;
	float max_depth;
	float min_depth;
	bool SubSample;
	bool equalize;
	//declare any more private variables for the class

public:
	//declare objects as a pointer in this section
	//examples:
	//dockingportGSP dockport1;
	//otherPeripheralGSP otherPeripheral1;

	testproject ();
	void GSsetup();
	void GSrunMain();
	void GSinit();
	void GSparseParameterFile(string line);
	void GSbackgroundTask();
	void GScleanup();
	void GSparseCommandlineArgs(int argc, char *argv[]);

	//add any other methods here
};

#endif
