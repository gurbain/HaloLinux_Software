#ifndef _TESTPROJECT_H_
#define _TESTPROJECT_H_

#include "../../DockingPortCore/dockingportGSP.h"


using namespace cv;
class testproject : public dockingportGSP {

	char key;
    int maneuverNumber;
	pthread_mutex_t keymutex;
	unsigned char charKey;
	Mat smallImg;

	int testEndTimeSeconds;

	//video buffers
	MatVideoBuffer singleVideoBuffer;
	int counter;

	//StereoBM bm;
	Mat disp, disp8;
	Rect roi1, roi2;
	int SADWindowSize;
	int numberOfDisparities;
	Mat Q;

	//StereoSAD stereosad;

	struct timespec time1, time2;
	double delT, averageTime;
	int time_samples;
	double camera_delta_T, measurement_delta_T;
	float max_depth;
	float min_depth;

	Vec3d previous_meanvec;
	Vec3d previous2_meanvec;

	Vec3d previous_meanvel;
	Vec3d previous2_meanvel;

	// Camera Settings
	dockingport_Camera camera;
	//VideoStreaming videostreaming;

	// Triangulator
	//Triangulator* triangulator;
	double minDisparity, maxDisparity;


	// Dense Stereo
	//DenseStereo* denseStereo;

	bool SubSample;
	bool equalize;

	ofstream imu_log_file, forceTorque_log_file, posVel_log_file, inst_posVel_log_file;

	int save_frame_number;




public:

	testproject ();
	void GSsetup();
	void GSrunMain();
	void GSprocessSingleImage(cv::Mat& singleImg); // Single image processing science algorithms
	void GSparseParameterFile(string line);
	void GSbackgroundTask();
	void GScleanup();
	void GSparseCommandlineArgs(int argc, char *argv[]);

};


#endif
