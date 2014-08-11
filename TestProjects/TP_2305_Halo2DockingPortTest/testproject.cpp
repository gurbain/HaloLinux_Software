#include "testproject.h"

testproject::testproject () {
	maneuverNumber = 1;
	pthread_mutex_init(&keymutex, NULL);
}

void testproject::GSinit(){
	char UDP_serial_1 [11]= "14";
	char UDP_serial_2 [11]= "15";
	dockport1 = new dockingportGSP(this, UDP_serial_1);
	dockport2 = new dockingportGSP(this, UDP_serial_2);
}

void testproject::GSsetup(){
	dockport1->init();
	dockport2->init();

	this->videostreaming.createNew_MatVideoBuffer(combinedVideoBuffer,"Combined Image");
	this->videostreaming.setAsDefaultVideoMode(combinedVideoBuffer);

	save_frame_number = 0;

	this->initVideoStreaming();
	dockport1->captureAndRectifySingleImage(dockport1->singleImage);
	dockport2->captureAndRectifySingleImage(dockport2->singleImage);

}

void testproject::GSrunMain()
{
	dockport1->captureAndRectifySingleImage(dockport1->singleImage);
	dockport2->captureAndRectifySingleImage(dockport2->singleImage);
	this->GSprocessImages(dockport1->singleImage, dockport2->singleImage); //split screen for videostreaming

}

void testproject::GSprocessImages(cv::Mat& leftImage, cv::Mat& rightImage) //copied from TP_2302
{
	cv::Mat combined_img;

	if (videostreaming.videoStreamingOn)
	{
		Size size( leftImage.cols + rightImage.cols, leftImage.rows );

		//place two images side by side
		combined_img.create( size, CV_MAKETYPE(leftImage.depth(), 3) ); //creates combined img of size leftImage+rightImage

		cv::Mat imgLeft = combined_img( Rect(0, 0, leftImage.cols, leftImage.rows) ); //sets imgLeft to the left side of the combined_img
		cv::Mat imgRight = combined_img( Rect(leftImage.cols, 0, rightImage.cols, rightImage.rows) );
		
		cv::cvtColor( leftImage, imgLeft, CV_RGB2BGR); //colors in imgLeft from leftImage using the RGB2BGR filter
		cv::cvtColor( imgLeft, imgLeft, CV_BGR2RGB); //gets rid of filter by changing back to RGB
		cv::cvtColor( rightImage, imgRight, CV_RGB2BGR);
		cv::cvtColor( imgRight, imgRight, CV_BGR2RGB);

		videostreaming.setAsDefaultVideoMode(combinedVideoBuffer);
		videostreaming.update_MatVideoBuffer(combinedVideoBuffer, combined_img);
	}
}

void testproject::GSbackgroundTask() {

}

void testproject::GSparseParameterFile(string line) {
	  dockport1->parseParameterFile(line, true);
	  dockport2->parseParameterFile(line, false);
}

void testproject::GSparseCommandlineArgs(int argc, char *argv[]) {

// Read in path from environment variable
	sprintf(this->calibParamSetName, "%s", argv[3]);

	if (argc > 4) {
		//add other GS variables to be set by command line input
	}

}

void testproject::GScleanup() {

	dockport1->shutdown();
	dockport2->shutdown();
	delete dockport1;
	delete dockport2;
}
