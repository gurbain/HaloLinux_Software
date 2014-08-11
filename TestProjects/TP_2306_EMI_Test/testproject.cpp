#include "testproject.h"

using namespace cv;

testproject::testproject () {

	maneuverNumber = 1;
	pthread_mutex_init(&keymutex, NULL);
}

void testproject::GSinit(){

	HIDS opticscam = 1;
	char UDP_serial_1 [11] = "114";
	char UDP_serial_2 [11] = "115";

	//important to initialize the OM before DP to be able to connect to right cams
	opticsmount1 = new opticsmountGSP(this, opticscam);
	dockport1 = new dockingportGSP(this, UDP_serial_1);
	dockport2 = new dockingportGSP(this, UDP_serial_2);
}

void testproject::GSsetup(){

	//important to init OM first
	opticsmount1->init();
	dockport1->init();
	dockport2->init();

	/*
	//videoStreaming from 2304/5
	this->videostreaming.createNew_MatVideoBuffer(combinedVideoBuffer,"Combined Image"); //2305
	this->videostreaming.setAsDefaultVideoMode(combinedVideoBuffer); //2305

	save_frame_number = 0;

	videostreaming.createNew_MatVideoBuffer(doubleVideoBuffer, "OM Left DP Right"); //HL added from 2304
	videostreaming.setAsDefaultVideoMode(doubleVideoBuffer); //HL added from 2304

	//HL 2302 <  txt files written to GSData from 2302
	string imu_log_filename = opticsmount1->datastorage.getGSdatastoragePath() + "imu_data.txt"; 
	imu_log_file.open(imu_log_filename.c_str()); 

	string forceTorque_log_filename = opticsmount1->datastorage.getGSdatastoragePath() + "forceTorque_data.txt";
	forceTorque_log_file.open(forceTorque_log_filename.c_str());

	string globMet_log_filename = opticsmount1->datastorage.getGSdatastoragePath() + "globMet_data.txt";
	globMet_log_file.open(globMet_log_filename.c_str());
	//HL /2302>  end of stuff copied from TP_2302
	*/

	this->initVideoStreaming();

	//always run the captureAndRectify methods once in case first picture is bad, so rest are ok during the actual test
	opticsmount1->captureAndRectifyImages(opticsmount1->leftImage, opticsmount1->rightImage); //HL 2302
	dockport1->captureAndRectifySingleImage(dockport1->singleImage);
	dockport2->captureAndRectifySingleImage(dockport2->singleImage);
}

void testproject::GSrunMain()
{
	opticsmount1->captureAndRectifyImages(opticsmount1->leftImage, opticsmount1->rightImage); //HL 2302
	dockport1->captureAndRectifySingleImage(dockport1->singleImage); //2303
	dockport2->captureAndRectifySingleImage(dockport2->singleImage); //2303

	/*
	//right now, the GSprocess methods are updating videostreaming, UDP software is working on image processing
	this->GSprocessSingleImage(dockport1->singleImage);
	this->GSprocessSingleImage(dockport2->singleImage);

	//this->GSstreamImages(opticsmount1->leftImage, opticsmount1->rightImage, dockport1->singleImage); //1 UDP + 1 OM videostreaming (2304)
	//this->GSstreamImages(opticsmount1->leftImage, opticsmount1->rightImage, dockport2->singleImage);
	//this->GSprocessImages(opticsmount1->leftImage, opticsmount1->rightImage); //2 UDP videostreaming (2305)
	*/
}

void testproject::GSprocessGuiKeyPress(unsigned char networkkey) //HL 2302 added method
{
	pthread_mutex_lock(&keymutex);
	charKey = networkkey;
	pthread_mutex_unlock(&keymutex);
}

void testproject::GSbackgroundTask() {

}

void testproject::GSparseParameterFile(string line) { 

	dockport1->parseParameterFile(line, true);
	dockport2->parseParameterFile(line,false);
	opticsmount1->parseParameterFile(line, false);
}

void testproject::GScleanup() {
	
	dockport1->shutdown();
	dockport2->shutdown();
	opticsmount1->shutdown();

	// delete the pointers, memory management
	delete dockport1;
	delete dockport2;
	delete opticsmount1;
}

void testproject::GSparseCommandlineArgs(int argc, char *argv[]) { 

	//Read in path from environment variable
	sprintf(this->calibParamSetName, "%s", argv[3]);

	if (argc > 4) {

		//for more than 3 arguments, GS can set which variables they want to equal which command line inputs
		//all variables here should be members of Halo (and so also members of testproject)
		//so that they can be easily accessed within the testproject code as this->variablename

	}

}

void testproject::GSprocessIMU(double timestamp, msg_imu_data imu_data) { // HL 2302 added method
	imu_log_file << timestamp << "," << imu_data.testTime << ","  << imu_data.gyro[0] << "," << imu_data.gyro[1] << "," << imu_data.gyro[2] << "," << imu_data.accel[0] << "," << imu_data.accel[1] << "," << imu_data.accel[2] << std::endl;
}

void testproject::GSprocessForceTorque(double timestamp, msg_body_forces_torques FT_data) { //HL 2302 added method
	forceTorque_log_file << timestamp << "," << FT_data.testTime << ","  << FT_data.forces[0] << "," << FT_data.forces[1] << "," << FT_data.forces[2] << "," << FT_data.torques[0] << "," << FT_data.torques[1] << "," << FT_data.torques[2] << std::endl;
}

void testproject::GSprocessGlobalMetrology(double timestamp, msg_state gm_data) { //HL 2302 added method
	globMet_log_file << timestamp << "," << gm_data.testTime << ","
											<< gm_data.pos[0] << "," << gm_data.pos[1] << "," << gm_data.pos[2] << ","
											<< gm_data.vel[0] << "," << gm_data.vel[1] << "," << gm_data.vel[2] << ","
											<< gm_data.quat[0] << "," << gm_data.quat[1] << "," << gm_data.quat[2] << "," << gm_data.quat[3] << ","
											<< gm_data.angvel[0] << "," << gm_data.angvel[1] << "," << gm_data.angvel[2] << std::endl;
}

/*

//HL deleted out the following methods, because they are not being used:
//void testproject::GSprocessImages(Mat& leftImage, Mat& rightImage) from TP_2302
//void testproject::GSstreamImages(cv::Mat& leftImage,cv::Mat& rightImage,cv::Mat& singleImage) from TP_2304
void testproject::GSprocessImages(cv::Mat& leftImage, cv::Mat& rightImage)
 
//from 2305- videoStreaming for 2 UDPs
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

void testproject::GSprocessSingleImage(cv::Mat& singleImg){ //HL GSprocessSingleImage from 2303- updates videoStreaming
	
	if (videostreaming.videoStreamingOn) {
		videostreaming.update_MatVideoBuffer(singleVideoBuffer, singleImg);
	}
}
*/
