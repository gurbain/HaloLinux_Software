#include "testproject.h"

testproject::testproject () {
	maneuverNumber = 1;
	pthread_mutex_init(&keymutex, NULL);
}

void testproject::GSinit(){
	HIDS opticscam = 1;
	char UDP_serial [11]= "115";

	//important to initialize the OM before DP to be able to connect to right cams
	opticsmount1 = new opticsmountGSP(this, opticscam);
// 	dockport1 = new dockingportGSP(this, UDP_serial);
}

void testproject::GSsetup(){

	opticsmount1->init();
// 	dockport1->init();

// 	videostreaming.createNew_MatVideoBuffer(doubleVideoBuffer,"OM Left DP Right");
// 	videostreaming.setAsDefaultVideoMode(doubleVideoBuffer);
// 
// 	save_frame_number = 0;
// 
// 	initVideoStreaming();
	// Take one image for each to remove a possibly bad first image before GSrunMain
// 	dockport1->captureAndRectifySingleImage(dockport1->singleImage); //HL
	opticsmount1->captureAndRectifyImages(opticsmount1->leftImage,opticsmount1->rightImage); //HL

}

void testproject::GSrunMain()
{
// 	dockport1->captureAndRectifySingleImage(dockport1->singleImage); //HL
	opticsmount1->captureAndRectifyImages(opticsmount1->leftImage,opticsmount1->rightImage); //HL
	this->GSstreamImages(opticsmount1->leftImage,opticsmount1->rightImage); //HL ?might be wrong
}

void testproject::GSstreamImages(cv::Mat& leftImage,cv::Mat& rightImage) { //HL changed Img to Image
	Mat doubleImg = rightImage;
	
/*
	Size size( leftImage.cols + singleImage.cols, MAX(leftImage.rows, singleImage.rows) ); //HL again, change Img to Image

	//place two images side by side
	doubleImg.create( size, CV_MAKETYPE(leftImage.depth(), 3) );
	Mat doubleImgLeft = doubleImg( Rect(0, 0, leftImage.cols, leftImage.rows) );
	Mat doubleImgRight = doubleImg( Rect(leftImage.cols, 0, singleImage.cols, singleImage.rows) );*/

	//cvtColor( leftImage, doubleImgLeft, CV_GRAY2BGR );
	//singleImage.copyTo(doubleImgRight);

	if (opticsmount1->datastorage.autoImageStorage) {
			stringstream filename;
			filename << opticsmount1->datastorage.getGSdatastoragePath() << "SideBySideImg" << save_frame_number << ".bmp";
			cv::imwrite(filename.str(), doubleImg);
			filename.clear();
/*
			filename << opticsmount1->datastorage.getGSdatastoragePath() << "SingleImg" << save_frame_number << ".bmp";
			cv::imwrite(filename.str(), singleImage);
			filename.clear();*/

			filename << opticsmount1->datastorage.getGSdatastoragePath() << "RightImg" << save_frame_number << ".bmp";
			cv::imwrite(filename.str(), rightImage);
			filename.clear();

			save_frame_number++;
	}
/*
	if (videostreaming.videoStreamingOn) {
		videostreaming.update_MatVideoBuffer(doubleVideoBuffer, doubleImg);
	}*/

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

// 	  dockport1->parseParameterFile(line, true);
	  opticsmount1->parseParameterFile(line, false);
}

void testproject::GScleanup() {
	
// 	dockport1->shutdown();
	opticsmount1->shutdown();
// 	delete dockport1;
	delete opticsmount1;
}
