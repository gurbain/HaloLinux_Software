#ifndef __DP_RECTIFIER__

#define __DP_RECTIFIER__

#define __LINUX__

#include <stdio.h>
#include <string.h>
#include <iostream>

// uEye camera library
#include <uEye.h>
#include <ueye_deprecated.h>

// OpenCV
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

// C++ includes
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;


// create object rectifier, that calls functions to undistort and rectifify stereo images
class dockingport_Rectifier
{
//	const char* intrinsic_filename;
//	const char* extrinsic_filename;

	char intrinsic_filename[200];
	char extrinsic_filename[200];

	cv::Mat LeftRectMap1, LeftRectMap2;
	cv::Mat RightRectMap1, RightRectMap2;

	cv::Mat Q;
	cv::Mat R, T, R1, P1, R2, P2;
	cv::Mat M1, D1, M2, D2;
    double Tx, Ty, Tz;
	double f;
	double cx;
	double cy;
	cv::Rect roi1, roi2;


public:

    bool rectifierOn;

	dockingport_Rectifier()	{

		rectifierOn = true;

	}

	int calcRectificationMaps(int imgwidth, int imgheight, const char calibParamDir[200]);

	int rectifyImages(cv::Mat& leftImgFrame, cv::Mat& rightImgFrame);

	void getCameraParameters(cv::Mat& Qin, cv::Mat& Rin, cv::Mat& Tin, cv::Mat& R1in, cv::Mat& P1in,
			cv::Mat& R2in, cv::Mat& P2in, cv::Mat& M1in, cv::Mat& D1in, cv::Mat& M2in, cv::Mat& D2in,
			double& Txin, double& Tyin, double& Tzin, double& fin, double& cxin, double& cyin);

	void getCameraParameters(cv::Mat& Rin, cv::Mat& Tin, cv::Mat& M1in, cv::Mat& D1in, cv::Mat& M2in, cv::Mat& D2in);

	void getCameraParameters(cv::Mat& Qin);

	void getCameraParameters(double & cx_left,double &  cy_left,double &  cx_right,double &  cy_right,
			double &  f_left,double &  f_right,double &  Tx,double &  Ty,double &  Tz);

};

#endif


