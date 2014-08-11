#include "testproject.h"

testproject::testproject () {

	maneuverNumber = 1;
	pthread_mutex_init(&keymutex, NULL);
}

void testproject::GSrunMain()  
{
	opticsmount1->captureAndRectifyImages(opticsmount1->leftImage,opticsmount1->rightImage);
	this->GSstreamImages(opticsmount1->leftImage, opticsmount1->rightImage);		
	this->GSprocessImages(opticsmount1->leftImage, opticsmount1->rightImage);
}

void testproject::GSinit()
{
	HIDS opticscam = 1;
	opticsmount1 = new opticsmountGSP(this, opticscam);
}

void testproject::GSsetup(){

	opticsmount1->init();
	this->videostreaming.createNew_MatVideoBuffer(combinedVideoBuffer,"Combined Image");
	this->videostreaming.setAsDefaultVideoMode(combinedVideoBuffer);

	save_frame_number = 0;

	this->initVideoStreaming();
	toggle = 0;
	toggle2 = 0;

	//phase1.GSsetup(opticsmount1); // RS commented out because it might've been the source of the problem

	string imu_log_filename = opticsmount1->datastorage.getGSdatastoragePath() + "imu_data.txt";
	imu_log_file.open(imu_log_filename.c_str());

	string forceTorque_log_filename = opticsmount1->datastorage.getGSdatastoragePath() + "forceTorque_data.txt";
	forceTorque_log_file.open(forceTorque_log_filename.c_str());

	string globMet_log_filename = opticsmount1->datastorage.getGSdatastoragePath() + "globMet_data.txt";
	globMet_log_file.open(globMet_log_filename.c_str());

	opticsmount1->captureAndRectifyImages(opticsmount1->leftImage, opticsmount1->rightImage);
}

void testproject::GScleanup() {
	imu_log_file.close();
	forceTorque_log_file.close();
	globMet_log_file.close();
	//phase1.GScleanup(); //commented out pretty much everything to do with phase1
	opticsmount1->shutdown();
	delete opticsmount1;
}

void testproject::GSprocessImages(Mat& leftImage, Mat& rightImage) {
	//phase1.GSprocessImages(leftImage, rightImage);
	pthread_mutex_lock(&keymutex);
	if (charKey == 0x1B) { // ESC
		printf("Quitting...\n");
		shutdownCriterion = true;
	}
	charKey = 0x00;
	pthread_mutex_unlock(&keymutex);
}

void testproject::GSprocessGuiKeyPress(unsigned char networkkey) {
	pthread_mutex_lock(&keymutex);
	charKey = networkkey;
	pthread_mutex_unlock(&keymutex);
}

void testproject::GSparseParameterFile(string line) {

	opticsmount1->parseParameterFile(line, true);
	// phase1.GSparseParameterFile(line);
}

void testproject::GSparseCommandlineArgs(int argc, char *argv[]) {

// Read in path from environment variable
		sprintf(this->calibParamSetName, "%s", argv[3]);

	if (argc>4) {
		//add other GS variables to be set by command line input
	}
}

void testproject::GSstreamImages(cv::Mat& leftImage,cv::Mat& rightImage){ //HL changed Img to Image
	Mat doubleImg;
	Mat combined_img;
	Mat upsidedown_img;

	if (videostreaming.videoStreamingOn)
	{
		videostreaming.update_MatVideoBuffer(doubleVideoBuffer, doubleImg);

		Size size( leftImage.cols + rightImage.cols, leftImage.rows );

		//place two images side by side
		combined_img.create( size, CV_MAKETYPE(leftImage.depth(), 3) );
		Mat imgLeft = combined_img( Rect(0, 0, leftImage.cols, leftImage.rows) );
		Mat imgRight = combined_img( Rect(leftImage.cols, 0, rightImage.cols, rightImage.rows) );

		cvtColor( leftImage, imgLeft, CV_GRAY2BGR );
		cvtColor( rightImage, imgRight, CV_GRAY2BGR );

		Mat smaller;

		pyrDown(combined_img, smaller, Size(combined_img.cols/2, combined_img.rows/2));

		flip(smaller, upsidedown_img,0);

		if (toggle < 5) {
			videostreaming.setAsDefaultVideoMode(combinedVideoBuffer);
		} 
		else {
			videostreaming.setAsDefaultVideoMode();
		}
		toggle = toggle == 10 ? 0 : toggle+1;

		videostreaming.update_MatVideoBuffer(combinedVideoBuffer, smaller);
	}

}

void testproject::GSprocessIMU(double timestamp, msg_imu_data imu_data) {
	imu_log_file << timestamp << "," << imu_data.testTime << ","  << imu_data.gyro[0] << "," << imu_data.gyro[1] << "," << imu_data.gyro[2] << "," << imu_data.accel[0] << "," << imu_data.accel[1] << "," << imu_data.accel[2] << std::endl;
}

void testproject::GSprocessForceTorque(double timestamp, msg_body_forces_torques FT_data) {
	forceTorque_log_file << timestamp << "," << FT_data.testTime << ","  << FT_data.forces[0] << "," << FT_data.forces[1] << "," << FT_data.forces[2] << "," << FT_data.torques[0] << "," << FT_data.torques[1] << "," << FT_data.torques[2] << std::endl;
}

void testproject::GSprocessGlobalMetrology(double timestamp, msg_state gm_data) {
	globMet_log_file << timestamp << "," << gm_data.testTime << ","
											<< gm_data.pos[0] << "," << gm_data.pos[1] << "," << gm_data.pos[2] << ","
											<< gm_data.vel[0] << "," << gm_data.vel[1] << "," << gm_data.vel[2] << ","
											<< gm_data.quat[0] << "," << gm_data.quat[1] << "," << gm_data.quat[2] << "," << gm_data.quat[3] << ","
											<< gm_data.angvel[0] << "," << gm_data.angvel[1] << "," << gm_data.angvel[2] << std::endl;
}
