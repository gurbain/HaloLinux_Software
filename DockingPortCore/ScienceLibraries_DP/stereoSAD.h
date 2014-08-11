#ifndef STEREOSAD_H_
#define STEREOSAD_H_

//#include "cv.h"
//#include "highgui.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;
//using namespace cv;


class StereoSAD{

	cv::StereoBM bm; // the block matcher

public:

	cv::Mat disp, disp8;
	cv::Rect roi1, roi2;

	int SADWindowSize;
	int numberOfDisparities;
	int minDisparity;

	StereoSAD()
	{
		SADWindowSize = 9;
		numberOfDisparities = 112;
		minDisparity = 0;
	}

	void computeDisparity(cv::Mat& cvFrame1, cv::Mat& cvFrame2, cv::Mat& disparityMap);
	void computeDisparity(cv::Mat& cvFrame1, cv::Mat& cvFrame2, cv::Mat& disparityMap, cv::Mat& disp);
};

#endif
