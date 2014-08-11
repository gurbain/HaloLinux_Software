/*
 * GlobalFeatureMatcher.h
 *
 *  Created on: Mar 23, 2012
 *      Author: muggler
 */

#ifndef GLOBALFEATUREMATCHER_H_
#define GLOBALFEATUREMATCHER_H_

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

// OpenCV
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "FrameCatalogue.h"
#include "FrameMatcher.h"
#include "GlobalFeatureMap.h"

//#include "HelperFunctions.h"
#include "Logger.h"

struct GlobalMatch {
	int frameId1, frameId2;
	int featureId1, featureId2;
};

struct GlobalFeature {
	int frameId;
	int featureId;
};

struct FrameMatch {
	int frameId1, frameId2;
};

// TODO: Merge with FrameMatcher class?
class GlobalFeatureMatcher {
public:
	GlobalFeatureMatcher();


//	void matchCurrentFrame();
//	void matchAllFrames();

	void matchAllFrames();
	void matchAllToAllFrames();
	void matchFrameWithAllPreviousFrames(int frameId);
	void matchLastFrameWithAllPreviousFrames();
	void matchFrames(int frameId1, int frameId2);

	void linkMatches();
	void generateGlobalFeatureMap(const std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > globalFeaturePoints);

	void setFrameMatcher(FrameMatcher & _frameMatcher);
	void setFrameCatalogue(FrameCatalogue & _frameCatalogue);
	void setGlobalFeatureMap(GlobalFeatureMap & _globalFeatureMap);
	void setLogger(Logger & _logger);

	void saveGlobalFeatureImages(std::string & dirname);
	void updateVideoImagesWithFeatures(int frameNumber, std::vector<cv::Point2d> currLeft2dKeypoints,std::vector<cv::Point2d> currRight2dKeypoints,
			std::vector<cv::Point2d> prevLeft2dKeypoints,std::vector<cv::Point2d> prevRight2dKeypoints,
			std::vector<cv::Point2d> allCurrLeft2dKeypoints,
			std::vector<cv::Point2d> allCurrRight2dKeypoints,
			std::string& dirname);


	int getNumberOfMatches();

	// TODO: make private
	std::vector< std::vector<GlobalFeature> > globalFeatures;
	std::vector<GlobalMatch> globalMatches;
	std::vector<FrameMatch> frameMatches;

	double viewpointAngleChangeThreshold;
	double viewpointDistanceChangeThreshold;

	// 32-bit issue
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
	GlobalFeatureMap* globalFeatureMap;
	FrameCatalogue* frameCatalogue;
	FrameMatcher* frameMatcher;
	Logger* logger;
};

#endif /* GLOBALFEATUREMATCHER_H_ */
