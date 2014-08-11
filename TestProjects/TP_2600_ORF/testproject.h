#ifndef _TESTPROJECT_H_
#define _TESTPROJECT_H_

#include "../../OrfCore/orfGSP.h"
#include "../../OpticsMountCore/opticsmountGSP.h"
#include "utils.h"

using namespace cv;

class testproject : virtual public haloGSP {

	private:
		// Test parameters
		char key;
		int maneuverNumber;
		pthread_mutex_t keymutex;
		unsigned char charKey;
		
		// Saving files
		string save_dir;
		ofstream tsfile;
		string timestamps;
		int imgNum;
		int tslast;
		TimeStamp ts;
		
		// Video buffers
		MatVideoBuffer leftBuffer, rightBuffer, depthBuffer, visualBuffer, confidencyBuffer;
		
		// IMU save parameters
		//ofstream imu_log_file, forceTorque_log_file, posVel_log_file, inst_posVel_log_file;
		int save_frame_number;

	public:
		orfGSP *orf1;
		opticsmountGSP *opticsmount1;

		testproject ();
		void GSinit();
		void GSsetup();
		void GSrunMain();
		void GSparseParameterFile(string line);
		void GSbackgroundTask();
		void GScleanup();
		void GSparseCommandlineArgs(int argc, char *argv[]);
		void GSsaveOpticsMount(cv::Mat& leftImg, cv::Mat& rightImg);
	//	void GSstreamImages(cv::Mat& leftImg, cv::Mat& rightImg,cv::Mat& depthImg, cv::Mat& visualImg, cv::Mat& confidencyImg);
};

#endif
