#ifndef _TESTPROJECT_H_
#define _TESTPROJECT_H_

#include "../../DockingPortCore/dockingportGSP.h"
#include "../../OpticsMountCore/opticsmountGSP.h"

using namespace cv;
class testproject : virtual public haloGSP {

private:
	char key;
    int maneuverNumber;
	pthread_mutex_t keymutex;
	unsigned char charKey;
	Mat smallImg;

	int testEndTimeSeconds;

	//video buffers
	MatVideoBuffer doubleVideoBuffer;
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

	ofstream imu_log_file, forceTorque_log_file, posVel_log_file, inst_posVel_log_file;

	int save_frame_number;

public:
// 	dockingportGSP *dockport1;
	opticsmountGSP *opticsmount1;

	testproject ();
	void GSinit();
	void GSsetup();
	void GSrunMain();
	void GSparseParameterFile(string line);
	void GSbackgroundTask();
	void GScleanup();
	void GSparseCommandlineArgs(int argc, char *argv[]);
	void GSstreamImages(cv::Mat& leftImg, cv::Mat& rightImg); // image processing science algorithms

};

#endif
