/*
 * Frame.cpp
 *
 *  Created on: Mar 19, 2012
 *      Author: muggler
 */

#include "Frame.h"

Frame::Frame() {
	keypoints3D.clear();
	matchedKeypoints3D.clear();
}

void Frame::addFeatures(const std::vector<cv::KeyPoint> & _leftKeypoints, const cv::Mat & _leftDescriptors,
		const std::vector<cv::KeyPoint> & _rightKeypoints, const cv::Mat & _rightDescriptors,
		const std::vector<cv::DMatch> & _stereoMatches) {

	// Clear vectors
	leftKeypoints.clear();
	rightKeypoints.clear();

	// Create matrices for descriptors
	leftDescriptors.create(_stereoMatches.size(), _leftDescriptors.cols, _leftDescriptors.type());
	rightDescriptors.create(_stereoMatches.size(), _rightDescriptors.cols, _rightDescriptors.type());

	for (int i=0; i < (int)_stereoMatches.size(); i++) {
		// Copy keypoints
		leftKeypoints.push_back(_leftKeypoints[_stereoMatches[i].queryIdx]);
		rightKeypoints.push_back(_rightKeypoints[_stereoMatches[i].trainIdx]);

		// Copy descriptors
		cv::Mat temp;
		temp = leftDescriptors.row(i);
		_leftDescriptors.row(_stereoMatches[i].queryIdx).copyTo(temp);
		temp = rightDescriptors.row(i);
		_rightDescriptors.row(_stereoMatches[i].trainIdx).copyTo(temp);
	}
}



double Frame::getViewpointAngleDifference(const Frame* other) {
	return getPoseAngle2(pose, other->pose);
}

double Frame::getViewpointDistanceDifference(const Frame*  other) {
	return getPoseDistance2(pose, other->pose);
}
