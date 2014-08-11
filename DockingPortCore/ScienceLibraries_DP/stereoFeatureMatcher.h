#ifndef STEREO_FEATURE_MATCHER_H_
#define STEREO_FEATURE_MATCHER_H_

//#include "cv.h"
//#include "highgui.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"

#include <iostream>
#include <cmath>
#include <cstdio>

//using namespace cv;
using namespace std;

class RANSACmatcherStereo {

public:

	RANSACmatcherStereo() {

	}

	void RANSACmatching_Homography(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures, vector<cv::KeyPoint>& leftFeaturesRANSAC,
			vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors, cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches,
			vector<cv::DMatch>& stereoFeatureMatchesRANSAC, double SDthreshold, int maxiterRANSAC, int numberOfSufficientMatches);

	void RANSACmatching_MultiHomography(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures, vector<cv::KeyPoint>& leftFeaturesRANSAC,
			vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors, cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches,
			vector<cv::DMatch>& stereoFeatureMatchesRANSAC, double SDthreshold, int numOfMinMatchesForGoodHom, int maxiterRANSAC,
			int numberOfSufficientMatches);

	void RANSACmatching_Fundamental(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures, vector<cv::KeyPoint>& leftFeaturesRANSAC,
			vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors, cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches,
			vector<cv::DMatch>& stereoFeatureMatchesRANSAC, double SDthreshold, int maxiterRANSAC, int numberOfSufficientMatches);

	double calcSquaredDifference_Homography(const cv::KeyPoint& oneLeftFeature, const cv::KeyPoint& oneRightFeature, cv::Mat& Homography);
	// Sum of Squared Differences

	double calcSquaredDifference_Fundamental(const cv::KeyPoint& oneLeftFeature, const cv::KeyPoint& oneRightFeature, cv::Mat& Homography);
	// Sum of Squared Differences

	void randSelectDescriptorMatches(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures, vector<cv::KeyPoint>& KeypointsSelect1,
			vector<cv::KeyPoint>& KeypointsSelect2, vector<cv::DMatch>& allMatches, vector<cv::DMatch>& matchesSel,
			int numRandomSelections, int numAvMatches); // numRandomSelections should be 4


};


class StereoMatcher : public RANSACmatcherStereo {


public:

	double SDthreshold;
	int maxiterRANSAC;
	int numberOfSufficientMatches;

	int numOfMinMatchesForGoodHom;

	StereoMatcher() {

		SDthreshold = 0.005;
		maxiterRANSAC = 500;
		numberOfSufficientMatches = 25;
		numOfMinMatchesForGoodHom = 15;

	}

	void initialStereoMatch(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures, cv::Mat& leftDescriptors,
			cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches, vector<cv::KeyPoint>& leftFeaturesPreMatch,
			vector<cv::KeyPoint>& rightFeaturesPreMatch);

	void getStereoInliersRANSAC_Homography(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
			vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors,
			cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches, vector<cv::DMatch>& stereoFeatureMatchesRANSAC);

	void getStereoInliersRANSAC_MultiHomography(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
			vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors,
			cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches, vector<cv::DMatch>& stereoFeatureMatchesRANSAC);

	void getStereoInliersRANSAC_Fundamental(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures,
			vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, cv::Mat& leftDescriptors,
			cv::Mat& rightDescriptors, vector<cv::DMatch>& stereoFeatureMatches, vector<cv::DMatch>& stereoFeatureMatchesRANSAC);

	void getStereoMatches(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures, vector<cv::Point3f>& stereoPreMatched3Dcoord,
			cv::Mat& P1, vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, vector<cv::DMatch>& stereoFeatureMatches,
			vector<cv::DMatch>& stereoFeatureMatchesRANSAC);

	void getStereoMatches_Essential(vector<cv::KeyPoint>& leftFeatures, vector<cv::KeyPoint>& rightFeatures, cv::Mat& E,
			vector<cv::KeyPoint>& leftFeaturesRANSAC, vector<cv::KeyPoint>& rightFeaturesRANSAC, vector<cv::DMatch>& stereoFeatureMatches,
			vector<cv::DMatch>& stereoFeatureMatchesRANSAC);


};

#endif
