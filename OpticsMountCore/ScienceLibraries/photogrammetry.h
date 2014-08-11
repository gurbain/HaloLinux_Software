#ifndef PHOTOGRAMMETRY_H_
#define PHOTOGRAMMETRY_H_

//#include "cv.h"
//#include "highgui.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include "../opticsmount_Cameras.h"
#include "../opticsmount_Rectifier.h"
using namespace std;
//using namespace cv;

class Photogrammetry {
	double pixelSize;
	cv::Mat R, T, M_left, D_left, M_right, D_right;
	double cx_left, cy_left;
	double cx_right, cy_right;
	double f_left, f_right;
	double Tx, Ty, Tz;

	void center2dPoints(vector<cv::Point2f> & imgPoints, bool left);

public:

	Photogrammetry(opticsmount_Rectifier & rectifier)
	{
		pixelSize = 0.000006;
		void getCameraParameters(cv::Mat& Qin, cv::Mat& Rin, cv::Mat& Tin, cv::Mat& R1in, cv::Mat& P1in, cv::Mat& R2in, cv::Mat& P2in,
				cv::Mat& M1in, cv::Mat& D1in, cv::Mat& M2in, cv::Mat& D2in, double& Txin, double& Tyin, double& Tzin,
				double& fin, double& cxin, double& cyin);

		rectifier.getCameraParameters(cx_left, cy_left, cx_right, cy_right, f_left, f_right, Tx, Ty, Tz);
/*
		f_left = 0.0028/pixelSize;
		f_right = 0.0028/pixelSize;
		Tx = -0.081/pixelSize;
*/
	}

	// returns 3D Point Positions wrt upper left corner of left image
	int triangulate(const cv::Point2f & left_corners, const cv::Point2f & right_corners, cv::Point3d & PXX);

	//solves Absolute Orientation Problem using Horn's Quaternion Method
	int absoluteOrientation(vector<cv::Point3d> & left, vector<cv::Point3d> & right, cv::Mat & RMat, cv::Mat & TMat, double & scale);

	//solves Exterior Orientation Problem using Haralick's Globally convergent Iterative Method
	int exteriorOrientation(vector<cv::Point2f> & imgPoints, vector<cv::Point3d> & objPoints, cv::Mat & RMat, cv::Mat & TMat,
			vector<double> & d, double & meanSquaredError,
			bool usingLeft, int maxIterations, double errorThreshold);
};

#endif
