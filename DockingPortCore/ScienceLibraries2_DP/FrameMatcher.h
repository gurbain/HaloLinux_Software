/*
 * FrameMatcher.h
 *
 *  Created on: Mar 23, 2012
 *      Author: muggler
 */

#ifndef FRAMEMATCHER_H_
#define FRAMEMATCHER_H_

#include <iostream>

// OpenCV
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <Eigen/StdVector>

#include "Frame.h"
#include "FeatureMatcher.h"
#include "GlobalFeatureMap.h"

#include "AbsoluteOrientation.h"

class FrameMatcher {
public:
	FrameMatcher();

//	Eigen::Matrix4d absoluteOrientation(std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > & left,
//			std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > & right);

	void match(Frame & currentFrame, Frame & lastFrame, std::vector<cv::DMatch> & supportingMatches,
			Eigen::Matrix4d & motionEstimate);

	Eigen::Matrix4d getCameraPose(Frame & frame);

	// 32-bit issue
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	void setFeatureMatcher(FeatureMatcher & _featureMatcher);
	void setGlobalFeatureMap(GlobalFeatureMap & _globalFeatureMap);

	// Parameters
	int frameToFrameRANSACMaxIterations, frameToFrameRANSACMinInliers;
	double frameToFrameRANSACMaxDistanceForInliers, frameToFrameRANSACBreakRatio;

	int cameraPoseEstimationRANSACMaxIterations, cameraPoseEstimationRANSACMinInliers;
	double cameraPoseEstimationRANSACMaxDistanceForInliers, cameraPoseEstimationRANSACBreakRatio;


private:
	FeatureMatcher *featureMatcher;
	GlobalFeatureMap *globalFeatureMap;
};

#endif /* FRAMEMATCHER_H_ */
