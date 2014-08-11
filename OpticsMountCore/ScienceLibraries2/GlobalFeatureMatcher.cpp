/*
 * GlobalFeatureMatcher.cpp
 *
 *  Created on: Mar 23, 2012
 *      Author: muggler
 */

#include "GlobalFeatureMatcher.h"

GlobalFeatureMatcher::GlobalFeatureMatcher() {
	// Preset values
	viewpointAngleChangeThreshold = M_PI/2.; // [rad]
	viewpointDistanceChangeThreshold = 10.0; // [m]
}

void GlobalFeatureMatcher::matchAllFrames() {
	logger->log("Global Feature Matching");
	for (int i=1; i<frameCatalogue->getNumberOfFrames(); i++) {
		matchFrameWithAllPreviousFrames(i);
		logger->log(".", false);
	}
	logger->newLine();
}

void GlobalFeatureMatcher::matchAllToAllFrames() {
	logger->log("Global All2All Feature Matching");
	for (int i=0; i<frameCatalogue->getNumberOfFrames(); i++) {
		for (int j=0; j<frameCatalogue->getNumberOfFrames(); j++) {
			if (i != j) {
				matchFrames(i,j);
			}
			logger->log(".", false);
		}
	}
	logger->newLine();
}

void GlobalFeatureMatcher::matchFrameWithAllPreviousFrames(int frameId) {
	for (int i=0; i<frameId; i++) {
		matchFrames(frameId, i);
	}
}

void GlobalFeatureMatcher::matchLastFrameWithAllPreviousFrames() {
	matchFrameWithAllPreviousFrames(frameCatalogue->getNumberOfFrames() - 1);
}

void GlobalFeatureMatcher::matchFrames(int frameId1, int frameId2) {
	Frame* frame1 = frameCatalogue->getFrame(frameId1);
	Frame* frame2 = frameCatalogue->getFrame(frameId2);

	// Only check view point change if pose estimate is available
	double viewpointAngleChange = 0.;
	double viewpointDistanceChange = 0.;
/*	if (!frame1->pose(3, 3) > 0.5 && !frame2->pose(3, 3) > 0.5) {
		viewpointAngleChange = frame1->getViewpointAngleDifference(frame2);
		viewpointDistanceChange = frame1->getViewpointDistanceDifference(frame2);
	}
*/
	//this test does nothing
	if (viewpointAngleChange < viewpointAngleChangeThreshold && viewpointDistanceChange < viewpointDistanceChangeThreshold) {

		std::cout << "Frame Match [" << frameId1 << " , " << frameId2 << "] - ";
		std::vector<cv::DMatch> matches;
		Eigen::Matrix4d motionEstimate;
		frameMatcher->match(*frame1, *frame2, matches, motionEstimate);

		if (matches.size() > 0) {
			// Add match to frames, if not already there
			if (std::find(frame1->linkedFrames.begin(), frame1->linkedFrames.end(), frameId2) == frame1->linkedFrames.end()) {
				frame1->linkedFrames.push_back(frameId2);
				//frame1->linkedFramesEstimate.push_back(motionEstimate.clone());
				frame1->linkedFramesEstimate.push_back(motionEstimate);
			}
			if (std::find(frame2->linkedFrames.begin(), frame2->linkedFrames.end(), frameId1) == frame2->linkedFrames.end()) {
				frame2->linkedFrames.push_back(frameId1);
				//frame2->linkedFramesEstimate.push_back(motionEstimate.clone().inv());
				frame2->linkedFramesEstimate.push_back(motionEstimate.inverse());
			}


			// Save all matches between to frame
			// TODO: use more sophisticated structure for more efficient processing later on
			for (int k=0; k < (int)matches.size(); k++) {
				GlobalMatch newMatch;
				newMatch.frameId1 = frameId1;
				newMatch.frameId2 = frameId2;
				newMatch.featureId1 = matches[k].queryIdx;
				newMatch.featureId2 = matches[k].trainIdx;

				globalMatches.push_back(newMatch);
			}
		}
	}
}


void GlobalFeatureMatcher::linkMatches() {
	globalFeatures.clear();

	for (int i=0; i < (int)globalMatches.size(); i++) {
		bool found = false;
		for (int j=0; j < (int)globalFeatures.size(); j++) {
			for (int k=0; k < (int)globalFeatures[j].size(); k++) {
				if (globalMatches[i].featureId1 == globalFeatures[j][k].featureId && globalMatches[i].frameId1 == globalFeatures[j][k].frameId) {
					GlobalFeature newGlobalFeature;
					newGlobalFeature.featureId = globalMatches[i].featureId2;
					newGlobalFeature.frameId = globalMatches[i].frameId2;
					globalFeatures[j].push_back(newGlobalFeature);
					found = true;
					break;
				} else if (globalMatches[i].featureId2 == globalFeatures[j][k].featureId && globalMatches[i].frameId2 == globalFeatures[j][k].frameId) {
					GlobalFeature newGlobalFeature;
					newGlobalFeature.featureId = globalMatches[i].featureId1;
					newGlobalFeature.frameId = globalMatches[i].frameId1;
					globalFeatures[j].push_back(newGlobalFeature);
					found = true;
					break;
				}
			}
			if (found)
				break;
		}

		// TODO: don't break, but check for consistency...

		if (!found) {
			GlobalFeature newGlobalFeature;
			std::vector<GlobalFeature> newGlobalFeatureVector;
			newGlobalFeature.featureId = globalMatches[i].featureId1;
			newGlobalFeature.frameId = globalMatches[i].frameId1;
			newGlobalFeatureVector.push_back(newGlobalFeature);
			newGlobalFeature.featureId = globalMatches[i].featureId2;
			newGlobalFeature.frameId = globalMatches[i].frameId2;
			newGlobalFeatureVector.push_back(newGlobalFeature);
			globalFeatures.push_back(newGlobalFeatureVector);
		}
	}
}

void GlobalFeatureMatcher::generateGlobalFeatureMap(const std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > globalFeaturePoints) {
	globalFeatureMap->descriptors = cv::Mat(	globalFeatures.size(),
												frameCatalogue->getFrame(0)->leftDescriptors.cols,
												frameCatalogue->getFrame(0)->leftDescriptors.type()
												);

	for (int i=0; i < (int)globalFeatures.size(); i++) {
		int frameId = globalFeatures[i][0].frameId;
		int featureId = globalFeatures[i][0].featureId;
		cv::Mat temp = globalFeatureMap->descriptors.row(i);
		frameCatalogue->getFrame(frameId)->leftDescriptors.row(featureId).copyTo(temp);
		globalFeatureMap->points.push_back(globalFeaturePoints[i]);
	}

//	std::cout << "globalFeatureMap->points.size(): " << globalFeatureMap->points.size() << "\nglobalFeatureMap->descriptors: \n" << globalFeatureMap->descriptors <<  std::endl;

}

void GlobalFeatureMatcher::saveGlobalFeatureImages(std::string & dirname) {
	std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > curr3dKeypoints, prev3dKeypoints;
	std::vector<cv::Point2d> currLeft2dKeypoints, currRight2dKeypoints, allCurrLeft2dKeypoints, allCurrRight2dKeypoints;
	std::vector<cv::Point2d> prevLeft2dKeypoints, prevRight2dKeypoints;
	std::vector<int> bestSupportingPoints;

	for (int frame_number=0; frame_number < (frameCatalogue->getNumberOfFrames()-1); frame_number++) {
		//currPose = new isam::Pose3d();
		curr3dKeypoints.clear();
		prev3dKeypoints.clear();
		currLeft2dKeypoints.clear();
		currRight2dKeypoints.clear();
		prevLeft2dKeypoints.clear();
		prevRight2dKeypoints.clear();
		bestSupportingPoints.clear();
		allCurrLeft2dKeypoints.clear();
		allCurrRight2dKeypoints.clear();


		for (int ii=0; ii < (int) globalFeatures.size(); ii++) {
			cv::Point2d currLeftKeypoint, currRightKeypoint, prevLeftKeypoint, prevRightKeypoint;
			bool foundCurr = false;
			bool foundPrev = false;
//			std::cout << "Size of Features: " << (int) globalFeatures[ii].size() << std::endl;

			for (int j=0; j < (int) globalFeatures[ii].size(); j++) {
				int frameId = globalFeatures[ii][j].frameId;
				int featureId = globalFeatures[ii][j].featureId;

				if (frameId == frame_number) {
					currLeftKeypoint = frameCatalogue->getFrame(frameId)->leftKeypoints[featureId].pt;
					currRightKeypoint = frameCatalogue->getFrame(frameId)->rightKeypoints[featureId].pt;
					foundCurr = true;
				} else if (frameId == frame_number + 1) {
					prevLeftKeypoint = frameCatalogue->getFrame(frameId)->leftKeypoints[featureId].pt;
					prevRightKeypoint = frameCatalogue->getFrame(frameId)->rightKeypoints[featureId].pt;
					foundPrev = true;
				}
//				std::cout << "[ii, j]: "<< ii << "," << j << "\nFrameId: " << frameId << "\nFeatureId: " << featureId << std::endl;
				if (foundCurr) {
					allCurrLeft2dKeypoints.push_back(currLeftKeypoint);
					allCurrRight2dKeypoints.push_back(currRightKeypoint);
				}

				if (foundCurr && foundPrev) {
					currLeft2dKeypoints.push_back(currLeftKeypoint);
					currRight2dKeypoints.push_back(currRightKeypoint);
					prevLeft2dKeypoints.push_back(prevLeftKeypoint);
					prevRight2dKeypoints.push_back(prevRightKeypoint);
					break;
				}
			}

		}
		updateVideoImagesWithFeatures(frame_number, currLeft2dKeypoints, currRight2dKeypoints,prevLeft2dKeypoints, prevRight2dKeypoints, allCurrLeft2dKeypoints, allCurrRight2dKeypoints, dirname);

	}
}

void GlobalFeatureMatcher::updateVideoImagesWithFeatures(
		int frameNumber,
		std::vector<cv::Point2d> currLeft2dKeypoints,
		std::vector<cv::Point2d> currRight2dKeypoints,
		std::vector<cv::Point2d> prevLeft2dKeypoints,
		std::vector<cv::Point2d> prevRight2dKeypoints,
		std::vector<cv::Point2d> allCurrLeft2dKeypoints,
		std::vector<cv::Point2d> allCurrRight2dKeypoints,
		std::string& dirname) {
	// If no error, stream images to ELC (ISS laptop)
	cv::Mat outImg2, rawImg;
	cv::Mat leftTemp, rightTemp;

	leftTemp = frameCatalogue->getFrame(frameNumber)->leftImage;
	rightTemp = frameCatalogue->getFrame(frameNumber)->rightImage;

	cv::Size size( (640+20)*2, (480+20));
	outImg2.create( size, CV_MAKETYPE(leftTemp.depth(), 3) );
	outImg2 = CV_RGB(0, 0, 0);
	rawImg.create( size, CV_MAKETYPE(leftTemp.depth(), 3) );
	rawImg = CV_RGB(0, 0, 0);

	// copy leftImage
	cv::Mat outImgLeft = outImg2( cv::Rect(10, 10, leftTemp.cols, leftTemp.rows) );
	cv::Mat outImgRight = outImg2( cv::Rect(670, 10, rightTemp.cols, rightTemp.rows) );
	cv::cvtColor( leftTemp, outImgLeft, CV_GRAY2BGR );
	cv::cvtColor( rightTemp, outImgRight, CV_GRAY2BGR );

	cv::Mat outImgLeft2 = rawImg( cv::Rect(10, 10, leftTemp.cols, leftTemp.rows) );
	cv::Mat outImgRight2 = rawImg( cv::Rect(670, 10, rightTemp.cols, rightTemp.rows) );
	cv::cvtColor( leftTemp, outImgLeft2, CV_GRAY2BGR );
	cv::cvtColor( rightTemp, outImgRight2, CV_GRAY2BGR );

	cv::Point2d leftoffset(10.0,10.0);
	cv::Point2d rightoffset(670.0,10.0);

	//draw keypoints
	cv::Scalar green = CV_RGB(0,255,0);
	cv::Scalar red = CV_RGB(255,0,0);
	cv::Scalar cyan = CV_RGB(0,255,255);
	cv::Scalar purple = CV_RGB(255,0,255);

	cv::Scalar color, dark_color;
	color = green;
	dark_color = purple;

	for (int i = 0; i < (int) allCurrLeft2dKeypoints.size(); i++) {
		cv::circle(outImgLeft,allCurrLeft2dKeypoints[i], 3, green);
		cv::circle(outImgRight,allCurrRight2dKeypoints[i], 3, green);
		cv::line(outImg2, allCurrLeft2dKeypoints[i]+rightoffset, allCurrRight2dKeypoints[i]+rightoffset,purple);
	}


	for (int i = 0; i < (int) currLeft2dKeypoints.size(); i++) {
		cv::Scalar color, dark_color;
		color = green;
		dark_color = purple;

//		cv::circle(outImgLeft,currLeft2dKeypoints[i], 3, green);
//		cv::circle(outImgRight,currRight2dKeypoints[i], 3, green);

		//stereo (epipolar) lines
//		cv::line(outImg2, currLeft2dKeypoints[i]+rightoffset, currRight2dKeypoints[i]+rightoffset,purple);

		//motion
		cv::line(outImgLeft, currLeft2dKeypoints[i], prevLeft2dKeypoints[i],red);
		//cv::line(outImgRight, currRight2dKeypoints[i], prevRight2dKeypoints[i],dark_color);
	}

	//videostreaming.update_MatVideoBuffer(checkoutCalibrationVideoBuffer2, outImg2);

	std::stringstream filename;
	filename << dirname << "GlobalFeaturesLR" << frameNumber << ".bmp";

	cv::imwrite(filename.str(), outImg2);

	std::stringstream filename2;
	filename2 << dirname << "OriginalLR" << frameNumber << ".bmp";

	cv::imwrite(filename2.str(), rawImg);
	std::cout << "Save frame number: " << frameNumber << std::endl;


	usleep(50000);

}


void GlobalFeatureMatcher::setFrameCatalogue(FrameCatalogue & _frameCatalogue) {
	frameCatalogue = &_frameCatalogue;
}

void GlobalFeatureMatcher::setFrameMatcher(FrameMatcher & _frameMatcher) {
	frameMatcher = &_frameMatcher;
}

int GlobalFeatureMatcher::getNumberOfMatches() {
	return globalMatches.size();
}

void GlobalFeatureMatcher::setGlobalFeatureMap(GlobalFeatureMap & _globalFeatureMap) {
	globalFeatureMap = &_globalFeatureMap;
}

void GlobalFeatureMatcher::setLogger(Logger & _logger) {
	logger = &_logger;
}
