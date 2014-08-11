#ifndef _TESTPROJECT_H_
#define _TESTPROJECT_H_

#include "haloGSP.h"
#include "../../OpticsMountCore/opticsmountGSP.h"
#include <stereoSAD.h>
#include <iostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <math.h>
#include <string>
#include <dirent.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>


#include "Triangulator.h"
#include "SimpleTriangulator.h"
#include "Camera.h"
#include "DenseStereo.h"

#include "elas_exception.h"


typedef struct {
	float xyzPos[3];
	float xyzVel[3];
} msg_body_tgt_location;

#define TGT_POSITION	0xE1

using namespace cv;
class testproject : public opticsmountGSP {

	// own declaration go here
	char key;
    	int maneuverNumber;
	pthread_mutex_t keymutex;
	unsigned char charKey;
	Mat smallImg;

	int testEndTimeSeconds;

	//video buffers
	MatVideoBuffer stereoVideoBuffer, blobVideoBuffer;
	int counter;

	StereoBM bm;
	Mat disp, disp8;
	Rect roi1, roi2;
	int SADWindowSize;
	int numberOfDisparities;
	Mat Q;

	StereoSAD stereosad;

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
	Camera camera;

	// Triangulator
	Triangulator* triangulator;
	double minDisparity, maxDisparity;


	// Dense Stereo
	DenseStereo* denseStereo;

	bool SubSample;
	bool equalize;

	ofstream imu_log_file, forceTorque_log_file, posVel_log_file, inst_posVel_log_file;

	int save_frame_number;

public:

	testproject ();
	void GSrunMain();
	void GSsetup();
	void blobTrack(Mat& leftImage, Mat& rightImage);
	void GSprocessImages(Mat& leftImage, Mat& rightImage);
	void GSprocessGuiKeyPress(unsigned char networkkey);
	void GSparseCommandlineArgs(int argc, char *argv[]);
	void GSparseParameterFile(string line);
	void GSprocessIMU(double timestamp, msg_imu_data imu_data);
	void GSprocessForceTorque(double timestamp, msg_body_forces_torques FT_data);
	void GScleanup();
	double timeDiff(struct timespec *start, struct timespec *end);
	double round (double value);

};

#endif
