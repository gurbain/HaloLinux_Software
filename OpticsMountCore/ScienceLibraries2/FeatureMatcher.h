/*
 * FeatureMatcher.h
 *
 *  Created on: Feb 14, 2012
 *      Author: muggler
 */

#ifndef FEATUREMATCHER_H_
#define FEATUREMATCHER_H_

// OpenCV include
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"


// Own classes
#include "Frame.h"


class FeatureMatcher {
public:
	FeatureMatcher();

	// Matchers
	void match(const cv::Mat& queryDescriptors, const cv::Mat& trainDescriptors, std::vector<cv::DMatch>& matches);

	void stereoMatch(const std::vector<cv::KeyPoint> & queryKeypoints, const cv::Mat & queryDescriptors,
			const std::vector<cv::KeyPoint> & trainKeypoints, const cv::Mat & trainDescriptors,
			std::vector<cv::DMatch> & matches, double maxVerticalOffset=1.,
			double minDisparity=0., double maxDisparity=256.);

	void viceVersaMatch(const cv::Mat& queryDescriptors, const cv::Mat& trainDescriptors, std::vector<cv::DMatch>& matches);

	// Checks
	void stereoSanityCheck(const std::vector<cv::KeyPoint>& leftKeypoints,
			const std::vector<cv::KeyPoint>& rightKeypoints, std::vector<cv::DMatch>& matches,
			double maxVerticalOffset=1., double minDisparity=0., double maxDisparity=256.);

	void viceVersaCheck(const std::vector<cv::DMatch> & matches1, const std::vector<cv::DMatch> & matches2,
			std::vector<cv::DMatch> & viceVersaMatches);

	void sameMatchesCheck(const std::vector<cv::DMatch> & matches1, const std::vector<cv::DMatch> & matches2,
			std::vector<cv::DMatch> & viceVersaMatches);

	// Setters
	void setDescriptorMatcher(cv::DescriptorMatcher & _descriptorMatcher);

private:
	cv::DescriptorMatcher* descriptorMatcher;
};

#endif /* FEATUREMATCHER_H_ */
