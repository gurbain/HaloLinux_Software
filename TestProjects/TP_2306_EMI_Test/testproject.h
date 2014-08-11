#ifndef _TESTPROJECT_H_
#define _TESTPROJECT_H_

#include "../../DockingPortCore/dockingportGSP.h"
#include "../../OpticsMountCore/opticsmountGSP.h" 

using namespace cv;

class testproject : virtual public haloGSP {
	//HL just made sure that every variable declared in 2302, 2304, and 2305 is declared here

	dockingportGSP *dockport1; //HL declare pointers
	dockingportGSP *dockport2;
	opticsmountGSP *opticsmount1;

	char key;
    int maneuverNumber;
	pthread_mutex_t keymutex;
	unsigned char charKey;
	Mat smallImg;

	int testEndTimeSeconds;

	//video buffers
	MatVideoBuffer singleVideoBuffer;
	MatVideoBuffer combinedVideoBuffer;
	MatVideoBuffer doubleVideoBuffer; //HL added from TP_2304
	int counter;

	//StereoBM bm;
	Mat disp, disp8;
	Rect roi1, roi2;
	int SADWindowSize;
	int numberOfDisparities;
	Mat Q;

	//StereoSAD stereosad;

	struct timespec time1, time2;
	double delT;
	double camera_delta_T, measurement_delta_T;

	Vec3d previous_meanvec;
	Vec3d previous2_meanvec;

	Vec3d previous_meanvel;
	Vec3d previous2_meanvel;

	// Triangulator
	//Triangulator* triangulator;
	double minDisparity, maxDisparity;

	// Dense Stereo
	//DenseStereo* denseStereo;

	ofstream imu_log_file, forceTorque_log_file, posVel_log_file, inst_posVel_log_file, globMet_log_file; //globMet added for 2302

	int save_frame_number;

public:

	testproject ();
	void GSinit(); //HL added
	void GSsetup();
	void GSrunMain(); 
	//void GSprocessSingleImage(cv::Mat& singleImg); // Single image processing science algorithms
	//void GSprocessImages(Mat& leftImage, Mat& rightImage); //Process two images (from two different cameras) simultaneously
	void GSparseParameterFile(string line);
	void GSbackgroundTask();
	void GScleanup(); //edited in cpp
	void GSparseCommandlineArgs(int argc, char *argv[]); //edited in cpp
	
	void GSstreamImages(cv::Mat& leftImage, cv::Mat& rightImage, cv::Mat& singleImage); //HL 2304
	void GSprocessGuiKeyPress(unsigned char networkkey); //HL TP_2302 single optics mount
	void GSprocessIMU(double timestamp, msg_imu_data imu_data); //HL 2302
	void GSprocessForceTorque(double timestamp, msg_body_forces_torques FT_data); // HL 2302
	void GSprocessGlobalMetrology(double timestamp, msg_state gm_data); // HL 2302
};

#endif
