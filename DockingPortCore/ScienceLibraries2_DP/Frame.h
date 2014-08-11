/*
 * Frame.h
 *
 *  Created on: Mar 19, 2012
 *      Author: muggler
 */

#ifndef FRAME_H_
#define FRAME_H_

#include <vector>

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>

// OpenCV
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

//#include "HelperFunctions.h"

inline double getPoseAngle2(const Eigen::Matrix4d pose1, const Eigen::Matrix4d pose2) {
	Eigen::Matrix3d R1 = pose1.block<3,3>(0,0);
	Eigen::Matrix3d R2 = pose2.block<3,3>(0,0);

	Eigen::Vector3d n;
	n << 1, 0, 0;

	Eigen::Vector3d n1 = R1*n;
	Eigen::Vector3d n2 = R2*n;

	double theta = acos(n1.dot(n2) / (n1.norm() * n2.norm()));

	return theta;
}

inline double getPoseDistance2(const Eigen::Matrix4d pose1, const Eigen::Matrix4d pose2) { //Frame*  other) {
	Eigen::Vector3d T1 = pose1.block<3,1>(0,3);
	Eigen::Vector3d T2 = pose2.block<3,1>(0,3);

	return (T2 - T1).norm();
}

class Frame {
public:
	Frame();

	void addFeatures(const std::vector<cv::KeyPoint> & _leftKeypoints, const cv::Mat & _leftDescriptors,
			const std::vector<cv::KeyPoint> & _rightKeypoints, const cv::Mat & _rightDescriptors,
			const std::vector<cv::DMatch> & _stereoMatches);

	double getViewpointAngleDifference(const Frame* other);
	double getViewpointDistanceDifference(const Frame* other);

	cv::Mat leftImage, rightImage;

	std::vector<cv::KeyPoint> leftKeypoints, rightKeypoints;
	cv::Mat leftDescriptors, rightDescriptors;

	std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > keypoints3D;
	std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > matchedKeypoints3D;

	// Matched Frames and their relative pose
	std::vector<int> linkedFrames;
	std::vector<double> linkedFramesScore;
	std::vector<Eigen::Matrix4d, Eigen::aligned_allocator<Eigen::Matrix4d> > linkedFramesEstimate;

	Eigen::Matrix4d pose;

	//cv::Mat disparityMap;
	std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > points;

	// 32-bit issue
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};

#endif /* FRAME_H_ */
