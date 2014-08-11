#include "testproject.h"

testproject::testproject () {
	maneuverNumber = 1;
	pthread_mutex_init(&keymutex, NULL);
}

void testproject::GSinit(){
	char UDP_serial [11]= "15";
	dockport1 = new dockingportGSP(this, UDP_serial); //currently connecting to UDP with serial ID of 15
}

void testproject::GSsetup(){
	dockport1->init();

	this->videostreaming.createNew_MatVideoBuffer(singleVideoBuffer,"Docking Port View");
	this->videostreaming.setAsDefaultVideoMode(singleVideoBuffer);

	save_frame_number = 0;

	this->initVideoStreaming();

	dockport1->captureAndRectifySingleImage(dockport1->singleImage);

}

void testproject::GSrunMain()
{
	dockport1->captureAndRectifySingleImage(dockport1->singleImage);
	this->GSprocessSingleImage(dockport1->singleImage);
}
	
void testproject::GSprocessSingleImage(cv::Mat& singleImg){

	if (videostreaming.videoStreamingOn) {
		videostreaming.update_MatVideoBuffer(singleVideoBuffer, singleImg);
	}
}

void testproject::GSbackgroundTask() {

}

void testproject::GSparseCommandlineArgs(int argc, char *argv[]) {

// Read in path from environment variable
	sprintf(this->calibParamSetName, "%s", argv[3]);

	if (argc > 4) {
		//add other GS variables to be set by command line input
	}
}

void testproject::GSparseParameterFile(string line) {

	dockport1->parseParameterFile(line, true);
}

void testproject::GScleanup() {
	
	dockport1->shutdown();
	delete dockport1;
}
