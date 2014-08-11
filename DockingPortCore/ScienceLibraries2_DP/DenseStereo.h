/*
 * DensePoints.h
 *
 *  Created on: Apr 2, 2012
 *      Author: muggler
 */

#ifndef DENSESTEREO_H_
#define DENSESTEREO_H_

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <exception>

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>

// OpenCV
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"

// libelas
#include "elas.h"

#include "Triangulator.h"
//#include "Frame.h"


class DenseStereo {
	cv::Mat nonthresh_disp;
	cv::Mat disparityImage;
	cv::Mat disparityImage_float;
public:

	cv::Mat getPreThreshDisp() {return nonthresh_disp;};
	DenseStereo(Triangulator & _triangulator, bool _subsample=false);

//	void calculate(const Eigen::Matrix4d & cameraPose, const cv::Mat & leftImage, const cv::Mat & rightImage);
//	cv::Mat calculate(const Eigen::Matrix4d & cameraPose, const cv::Mat & leftImage, const cv::Mat & rightImage);
//	void calculate(const cv::Mat & leftImage, const cv::Mat & rightImage);
	cv::Mat calculate(const cv::Mat & leftImage, const cv::Mat & rightImage);
//	void calculate(Frame* frame, bool useConvexHull=true, bool usePose=true);

	cv::Mat getDisparityImage(const cv::Mat & leftImage, const cv::Mat & rightImage);
	cv::Mat getDisparityMap(const cv::Mat & disparityImage);

	cv::Mat getDisparityImageFloat() { return disparityImage_float;}

//	void setupBM(int P1, int P2, int preFilterCap, int SADWindowSize, int uniquenessRatio, int speckleWindowSize, int speckleRange);

	void save(const std::string & filename, bool withColor=true);

	void clear();

	int getNumberOfPoints();
	Eigen::Vector3d getPoint(int i);

//	void setPixelStep(int _pixelStep) { pixelStep = _pixelStep; };
//	int getPixelStep() { return pixelStep; };

	// 32-bit issue
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	// TODO: make private and getter
	std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > points;
	std::vector<int> colors;

	Eigen::Vector3d meanPoints;

	bool getSubsample() { return subsample;}

private:
//	cv::StereoSGBM bm;
	Triangulator* triangulator;

//	bool useElas;

	int minDisparity;
	int maxDisparity;

//	int pixelStep;

	bool subsample;

	// temp temp
	float* D1_data;
	float* D2_data;
};

class DenseStereoException : public exception{
public:
	DenseStereoException(const string m="Exception Occurred in libELAS."):msg(m){}
	~DenseStereoException(void) throw() {}
	const char* what(){return msg.c_str();}
private:
           string msg;
};


#endif /* DENSESTEREO_H_ */
