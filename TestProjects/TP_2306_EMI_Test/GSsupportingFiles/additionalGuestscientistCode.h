#ifndef MYHEADERFILE_H_
#define MYHEADERFILE_H_

//#include "cv.h"
//#include "highgui.h"
#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sys/time.h>
#include <sys/resource.h>
#include <photogrammetry.h>

using namespace std;
using namespace cv;

#include <stereoSAD.h>
#include "stereoFeatureMatcher.h"


class GuestScientistClass{

    //// feature vectors and feature extractors
	vector<KeyPoint> leftFeatures, rightFeatures;
	int FASTthresh;
	int FASTthreshDummy;

    //// feature descriptors and descriptor extractor
	SurfDescriptorExtractor surfDescriptorExtractor;
	SiftDescriptorExtractor siftDescriptorExtractor;
	ORB orbDetector;
	SURF surfDetector;
	SIFT siftDetector;
	Mat leftDescriptors, rightDescriptors;

	//// feature stereo matching
	vector<DMatch> stereoFeatureMatches, stereoFeatureMatchesRANSAC;
	vector<KeyPoint> leftFeaturesPreMatch, rightFeaturesPreMatch;
	vector<KeyPoint> leftFeaturesRANSAC, rightFeaturesRANSAC;
	vector<KeyPoint> leftFeaturesRANSAC_filtered;
	vector<KeyPoint> rightFeaturesRANSAC_filtered;
	Mat leftDescriptorsRANSAC, rightDescriptorsRANSAC;
	int maxiterRANSAC;
	int numberOfSufficientMatches;
	vector<Point3d> stereoMatchedFeaturesRANSACcoord3D;
	vector<Point3d> stereoMatchedFeaturesRANSACcoord3D_transformed;

	//photogrammetry
	Photogrammetry* photo;

    Mat Q;
    Mat R, T, R1, P1, R2, P2;
    Mat M1, D1, M2, D2;
    double Tx;
	double f;
	double cx;
	double cy;

	int iterationNumber;

	vector<double> dval;
	bool dval_init;

public:

	StereoMatcher stereomatcher;
	StereoSAD stereoSAD;

	int imagewidth, imageheight;

	//// for visualization
	Mat featureMatchesImage;
	Mat featureMatchesImagePreRansac;

	// GSdata storage
	string GSstoragePath;
	ofstream positionVOEstimateFile;

	GuestScientistClass() {

		/// stereo RANSAC settings
		stereomatcher.SDthreshold = 0.20; // "abstract" value, since error = x_right' * F * x_left
		stereomatcher.maxiterRANSAC = 600;

		/// FAST settings
		FASTthresh = 40;

			/// other settings
		iterationNumber = 1;

		dval_init = false;

	}

	void extractFeatures(Mat& leftImage, Mat& rightImage);

	void extractFeatureDescriptors(Mat& leftImage, Mat& rightImage);

	void stereoMatchFeatures(Mat& leftImage, Mat& rightImage, int& numberMatches);

	void showStereoMatches(Mat& leftImage, Mat& rightImage, Mat& matchesImage);

	void updatePhotogrammetry(Rectifier & rectifier) {
		photo = new Photogrammetry(rectifier);
	}

	int triangulateChessboard(vector<Point2f>& leftImgPoints, vector<Point2f>& rightImgPoints,
			int chessboardWidth, int chessboardHeight, double & roll, double & pitch, double & yaw, Mat & Tout,
			double mean_squares[], double stddev_squares[]);

	int monocularChessboard(vector<Point2f>& imgPoints,
			int chessboardWidth, int chessboardHeight, bool usingLeft, double & roll, double & pitch, double & yaw, Mat & Tout, double & mse);

};

#endif
