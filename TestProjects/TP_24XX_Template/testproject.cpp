#include "testproject.h"  //be sure that this .h file is in same folder as testproject.cpp
//for an example, look at TP_2304_HaloOpticsAndXSCamTest/testproject.cpp to view what goes in each section

testproject::testproject () {
	//the constructor, sets some basic variables
	maneuverNumber = 1;
	averageTime = 0;
	time_samples = 0;
	max_depth = 100.0;
	min_depth = 0.01;
	SubSample = false;
	equalize = false;
	pthread_mutex_init(&keymutex, NULL);
	
}

void testproject::GSinit(){
	//include declaration of objects here
	//example:
	//char UDP_serial [11] = "14";
	//dockport1 = new dockingportGSP(this, UDP_serial);
}

void testproject::GSsetup(){
	//run initializing methods for objects declared
	//example:
	//dockport1->init();

}

void testproject::GSrunMain(){
	//include the main function of the test here

}

void testproject::GSbackgroundTask() {
	//secondary or background functions go here

}

void testproject::GSparseParameterFile(string line) {
	//declare parseParameterFile(line) methods for each peripheral here


    //Implement GS parameters here (follow commented out example)
	/*
	  string searchString;
	  string foundString;
	  size_t found;

	  searchString = "NEW_PARAMETER";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString == "true") {
			this->newParameter = true;
		}
		else {
			this->newParameter = false;
		cout << "NEW_PARAMETER:  " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;
	*/

}
	
void testproject::GScleanup() {

	//run shutdown functions for objects

	//del pointers for memory management
}

void testproject::GSparseCommandlineArgs(int argc, char *argv[]) {
//ensures that argument in command is valid
//Read in path from environment variable
	//the following line is an example of setting the input at index 3 to the variable calibParamSetName
	sprintf(this->calibParamSetName, "%s", argv[3]);

	if (argc > 4) {
		//add in GS command line parsing
		//example:
		//sprintf(this->someVariable, "%s", argv[4])

	}

}
