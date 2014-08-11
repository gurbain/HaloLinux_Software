/*
 * FrameMatcher.cpp
 *
 *  Created on: Mar 23, 2012
 *      Author: muggler
 */

#include "FrameMatcher.h"

FrameMatcher::FrameMatcher() {
	// TODO Auto-generated constructor stub

}

//uses RANSAC with Horn's Absolute Orientaiton on globalFeatures
//motionEstimate = [R T ; 0 1]
//globalFeatureMap = s R currentFrame + T
//current frame to globalFeature frame
Eigen::Matrix4d FrameMatcher::getCameraPose(Frame & frame) {
	Eigen::Matrix4d pose = Eigen::Matrix4d::Identity();

//	std::cout << "globalFeatureMap->points.size(): " << globalFeatureMap->points.size() << "\nglobalFeatureMap->descriptors: \n" << globalFeatureMap->descriptors <<  std::endl;

#define useold 0
#if useold
	std::vector<cv::DMatch> matches;
	featureMatcher->match(frame.leftDescriptors, globalFeatureMap->descriptors, matches);

	// try right descriptors if not enough
	if ((int)matches.size() < cameraPoseEstimationRANSACMinInliers)
		featureMatcher->match(frame.rightDescriptors, globalFeatureMap->descriptors, matches);

	std::cout << "frame.leftDescriptors.row/col: " << frame.leftDescriptors.rows << " , " << frame.leftDescriptors.cols << std::endl;
	std::cout << "frame.rightDescriptors.row/col: " << frame.rightDescriptors.rows << " , " << frame.rightDescriptors.cols << std::endl;
	std::cout << "matches.size(): " << matches.size() << std::endl;
#else
	std::vector<cv::DMatch> leftMatches, rightMatches, matches;
	featureMatcher->match(frame.leftDescriptors, globalFeatureMap->descriptors, leftMatches);
	featureMatcher->match(frame.rightDescriptors, globalFeatureMap->descriptors, rightMatches);
	featureMatcher->sameMatchesCheck(leftMatches, rightMatches, matches);

	// Not enough inliers
/*	std::cout << "frame.leftDescriptors.row/col: " << frame.leftDescriptors.rows << " , " << frame.leftDescriptors.cols << std::endl;
	std::cout << "frame.rightDescriptors.row/col: " << frame.rightDescriptors.rows << " , " << frame.rightDescriptors.cols << std::endl;
	std::cout << "leftMatches.size(): " << leftMatches.size() << std::endl;
	std::cout << "rightMatches.size(): " << rightMatches.size() << std::endl;
	for (int ii = 0; ii < leftMatches.size() && ii < rightMatches.size(); ii++) {
		std::cout << ii << ", [" << leftMatches[ii].queryIdx << "," << leftMatches[ii].trainIdx <<
				"] [" << rightMatches[ii].queryIdx << "," << rightMatches[ii].trainIdx << "]" << std::endl;
	}
	std::cout << "matches.size(): " << matches.size() << std::endl;
	for (int ii = 0; ii < matches.size(); ii++) {
		std::cout << ii << ", [" << matches[ii].queryIdx << "," << matches[ii].trainIdx << "]" << std::endl;
	}
*/
#endif

	if ((int)matches.size() < cameraPoseEstimationRANSACMinInliers) {
		std::cout << "Not enough inliers for getCameraPose()\n";
		return Eigen::Matrix4d::Identity();
	}

	int iterationNo = 0;

	std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > left, right;
	std::vector<int> supportingMatchesId, maxSupportingMatchesId;

	maxSupportingMatchesId.clear();

	while (iterationNo < cameraPoseEstimationRANSACMaxIterations) {

		// Select 4 points
		int pointIndex1 = rand() % matches.size();
		int pointIndex2 = rand() % matches.size();
		int pointIndex3 = rand() % matches.size();
		int pointIndex4 = rand() % matches.size();

		// Unique indices / distinct points?
		if (pointIndex1 == pointIndex2 || pointIndex1 == pointIndex3 || pointIndex1 == pointIndex4 ||
				pointIndex2 == pointIndex3 || pointIndex2 == pointIndex4 || pointIndex3 == pointIndex4)
			continue;

		// Count only "real" iterations
		iterationNo++;

		//std::cout << "iterationNo: " << iterationNo << std::endl;

		// Calculate guess
		left.clear();
		left.push_back(frame.keypoints3D[matches[pointIndex1].queryIdx]);
		left.push_back(frame.keypoints3D[matches[pointIndex2].queryIdx]);
		left.push_back(frame.keypoints3D[matches[pointIndex3].queryIdx]);
		left.push_back(frame.keypoints3D[matches[pointIndex4].queryIdx]);

		right.clear();
		right.push_back(globalFeatureMap->points[matches[pointIndex1].trainIdx]);
		right.push_back(globalFeatureMap->points[matches[pointIndex2].trainIdx]);
		right.push_back(globalFeatureMap->points[matches[pointIndex3].trainIdx]);
		right.push_back(globalFeatureMap->points[matches[pointIndex4].trainIdx]);

		Eigen::Matrix4d pose = AbsoluteOrientation::absoluteOrientation(left, right);

//		std::cout << "pose: " << pose << std::endl;

		supportingMatchesId.clear();

		// Count supporting matches for this guess
		for (int i=0; i < (int)matches.size(); i++) {
			Eigen::Vector4d oldLocationH;
			oldLocationH << frame.keypoints3D[matches[i].queryIdx], 1;

			Eigen::Vector4d newLocationH = pose*oldLocationH;
			Eigen::Vector3d newLocation(newLocationH(0), newLocationH(1), newLocationH(2));

			//std::cout << "norm dist: " << (newLocation - globalFeatureMap->points[matches[i].trainIdx]).norm() << std::endl;
			if ((newLocation - globalFeatureMap->points[matches[i].trainIdx]).norm() < cameraPoseEstimationRANSACMaxDistanceForInliers) {
				supportingMatchesId.push_back(i);
//				std::cout << "Accepted Reprojection distance: " << (newLocation - globalFeatureMap->points[matches[i].trainIdx]).norm() << std::endl;
			}
		}
		//std::cout << "supportingMatchesId.size(): " << supportingMatchesId.size() << std::endl;

		if (supportingMatchesId.size() > maxSupportingMatchesId.size())
			maxSupportingMatchesId = supportingMatchesId;

		if (maxSupportingMatchesId.size() > cameraPoseEstimationRANSACBreakRatio * matches.size())
			break;

	}

	std::cout << "Frame match iterations: " << iterationNo << ", ratio: " <<  maxSupportingMatchesId.size() <<  " / " << matches.size()  << " / " << frame.leftKeypoints.size() << std::endl;

	// if enough inliers
	left.clear();
	right.clear();
	frame.matchedKeypoints3D.clear();
	if ((int)maxSupportingMatchesId.size() >= cameraPoseEstimationRANSACMinInliers) {
		// optimise with all inliers
		for (int i=0; i < (int)maxSupportingMatchesId.size(); i++) {
			left.push_back(frame.keypoints3D[matches[maxSupportingMatchesId[i]].queryIdx]);
			right.push_back(globalFeatureMap->points[matches[maxSupportingMatchesId[i]].trainIdx]);
			frame.matchedKeypoints3D.push_back(frame.keypoints3D[matches[maxSupportingMatchesId[i]].queryIdx]);
		}
		pose = AbsoluteOrientation::absoluteOrientation(left, right);
	}

	return pose;
}

//uses RANSAC with Horn's Absolute Orientaiton on globalFeatures
//motionEstimate = [R T ; 0 1]
//lastFrame = s R currentFrame + T
void FrameMatcher::match(Frame & currentFrame, Frame & lastFrame,
		std::vector<cv::DMatch> & supportingMatches, Eigen::Matrix4d & motionEstimate) {

	supportingMatches.clear();
	motionEstimate = Eigen::Matrix4d::Identity();

	// match key frames
//	// take best solution from all four possible
//	// TODO: evaluate if it is worth the effort?!
//	std::vector<cv::DMatch> matches1, matches2, matches3, matches4, matches;
//	featureMatcher->viceVersaMatch(currentFrame.leftDescriptors, lastFrame.leftDescriptors, matches1);
//	featureMatcher->viceVersaMatch(currentFrame.rightDescriptors, lastFrame.rightDescriptors, matches2);
//	featureMatcher->viceVersaMatch(currentFrame.leftDescriptors, lastFrame.rightDescriptors, matches3);
//	featureMatcher->viceVersaMatch(currentFrame.rightDescriptors, lastFrame.leftDescriptors, matches4);
//
//	// take most matches
//	if (matches1.size() > matches2.size() && matches1.size() > matches3.size() && matches1.size() > matches4.size())
//		matches = matches1;
//	else if (matches2.size() > matches3.size() && matches2.size() > matches4.size())
//		matches = matches2;
//	else if (matches3.size() > matches4.size())
//		matches = matches3;
//	else
//		matches = matches4;

	// ... or just take left ones
	std::vector<cv::DMatch> matches;
	featureMatcher->viceVersaMatch(currentFrame.leftDescriptors, lastFrame.leftDescriptors, matches);

	// break if not enough matches
	if ((int)matches.size() < frameToFrameRANSACMinInliers)
		return;

	int iterationNo = 0;
	std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > left, right;
	std::vector<int> supportingMatchesId, maxSupportingMatchesId;

	maxSupportingMatchesId.clear();

	Eigen::Matrix4d pose;

	while (iterationNo < frameToFrameRANSACMaxIterations) {

		// Select 3 points
		int pointIndex1 = rand() % matches.size();
		int pointIndex2 = rand() % matches.size();
		int pointIndex3 = rand() % matches.size();
		int pointIndex4 = rand() % matches.size();

		// Distinct points?
		if (pointIndex1 == pointIndex2 || pointIndex1 == pointIndex3 || pointIndex1 == pointIndex4 ||
				pointIndex2 == pointIndex3 || pointIndex2 == pointIndex4 || pointIndex3 == pointIndex4)
			continue;

		// Count only "real" iterations
		iterationNo++;

		// Calculate guess
		left.clear();
		left.push_back(currentFrame.keypoints3D[matches[pointIndex1].queryIdx]);
		left.push_back(currentFrame.keypoints3D[matches[pointIndex2].queryIdx]);
		left.push_back(currentFrame.keypoints3D[matches[pointIndex3].queryIdx]);
		left.push_back(currentFrame.keypoints3D[matches[pointIndex4].queryIdx]);

		right.clear();
		right.push_back(lastFrame.keypoints3D[matches[pointIndex1].trainIdx]);
		right.push_back(lastFrame.keypoints3D[matches[pointIndex2].trainIdx]);
		right.push_back(lastFrame.keypoints3D[matches[pointIndex3].trainIdx]);
		right.push_back(lastFrame.keypoints3D[matches[pointIndex4].trainIdx]);

		// Don't use Horn's algorithm if not feasible
		// TODO: check if distances between points are within n % deviation
		// TODO: parameter n in cfg file
		// something like:
		// if (fabs (distance(left[0], left[1]) - distance(right[0], right[1])) > 0.1) continue;


		pose = AbsoluteOrientation::absoluteOrientation(left, right);

		supportingMatchesId.clear();

		// Count supporting matches for this guess
		for (int i=0; i < (int)matches.size(); i++) {
			Eigen::Vector4d oldLocationH;
			oldLocationH << currentFrame.keypoints3D[matches[i].queryIdx], 1;

			Eigen::Vector4d newLocationH = pose*oldLocationH;
			Eigen::Vector3d newLocation(newLocationH(0), newLocationH(1), newLocationH(2));

			if ((newLocation - lastFrame.keypoints3D[matches[i].trainIdx]).norm() < frameToFrameRANSACMaxDistanceForInliers) {
				supportingMatchesId.push_back(i);
			}
		}

		if (supportingMatchesId.size() > maxSupportingMatchesId.size())
			maxSupportingMatchesId = supportingMatchesId;

		if (maxSupportingMatchesId.size() > frameToFrameRANSACBreakRatio * matches.size())
			break;

	}
	std::cout << "Frame match iterations: " << iterationNo << ", ratio: " <<  maxSupportingMatchesId.size() <<  " / " << matches.size() << std::endl;

	// if enough inliers, optimize over all and return matches and motion estimate
	if ((int)maxSupportingMatchesId.size() >= frameToFrameRANSACMinInliers) {
		left.clear();
		right.clear();

		for (int i=0; i < (int)maxSupportingMatchesId.size(); i++) {
			supportingMatches.push_back(matches[maxSupportingMatchesId[i]]);

			left.push_back(currentFrame.keypoints3D[matches[maxSupportingMatchesId[i]].queryIdx]);
			right.push_back(lastFrame.keypoints3D[matches[maxSupportingMatchesId[i]].trainIdx]);
		}

//		std::cout << "Match size [" << left.size() << "," << right.size() << "]\n";

		motionEstimate = AbsoluteOrientation::absoluteOrientation(left, right);
	}
}

//// According to:
//// Berthold K. P. Horn: Closed-form solution of absolute orientation using unit quaternions (1986)
//// Implementation adapted from Brent Tweddle/Konrad Makowka (2011)
//Eigen::Matrix4d FrameMatcher::absoluteOrientation(std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > & left,
//		std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > & right) {
//
//	// set right dimensions of output matrix
//	Eigen::Matrix4d pose = Eigen::Matrix4d::Zero();
//
//	// compute the mean of the left and right set of points
//	Eigen::Vector3d leftMean = Eigen::Vector3d::Zero();
//	Eigen::Vector3d rightMean = Eigen::Vector3d::Zero();
//
//	for (int i = 0; i < (int)left.size(); i++) {
//		leftMean += left[i];
//		rightMean += right[i];
//	}
//
//	leftMean /= left.size();
//	rightMean /= right.size();
//
//	// Move all points to the "center"
//	for (int i = 0; i < (int)left.size(); i++) {
//		left[i] -= leftMean;
//		right[i] -= rightMean;
//	}
//
////	//compute scale (use the symmetrical solution)
////	double Sl = 0;
////	double Sr = 0;
////
////	// this is the symmetrical version of the scale !
////	for (int i = 0; i < (int)left.size(); i++) {
////		Sl += left[i].transpose() * left[i]; 	//left[i].x*left[i].x + left[i].y*left[i].y + left[i].z*left[i].z;
////		Sr += right[i].transpose() * right[i]; 	//right[i].x*right[i].x + right[i].y*right[i].y + right[i].z*right[i].z;
////	}
////	double scale = Sr/Sl;
//
//	//create M matrix
//	Eigen::Matrix3d M = Eigen::Matrix3d::Zero();
//
//	for (int i = 0; i < (int)left.size(); i++) {
//		M(0, 0) += left[i](0) * right[i](0);
//		M(0, 1) += left[i](0) * right[i](1);
//		M(0, 2) += left[i](0) * right[i](2);
//		M(1, 0) += left[i](1) * right[i](0);
//		M(1, 1) += left[i](1) * right[i](1);
//		M(1, 2) += left[i](1) * right[i](2);
//		M(2, 0) += left[i](2) * right[i](0);
//		M(2, 1) += left[i](2) * right[i](1);
//		M(2, 2) += left[i](2) * right[i](2);
//	}
//
//	//create N matrix
//	Eigen::Matrix4d N = Eigen::Matrix4d::Zero();
//	N(0, 0) = M(0, 0) + M(1, 1) + M(2, 2);
//	N(0, 1) = M(1, 2) - M(2, 1);
//	N(0, 2) = M(2, 0) - M(0, 2);
//	N(0, 3) = M(0, 1) - M(1, 0);
//
//	N(1, 0) = M(1, 2) - M(2, 1);
//	N(1, 1) = M(0, 0) - M(1, 1) - M(2, 2);
//	N(1, 2) = M(0, 1) + M(1, 0);
//	N(1, 3) = M(2, 0) + M(0, 2);
//
//	N(2, 0) = M(2, 0) - M(0, 2);
//	N(2, 1) = M(0, 1) + M(1, 0);
//	N(2, 2) = -M(0, 0) + M(1, 1) - M(2, 2);
//	N(2, 3) = M(1, 2) + M(2, 1);
//
//	N(3, 0) = M(0, 1) - M(1, 0);
//	N(3, 1) = M(2, 0) + M(0, 2);
//	N(3, 2) = M(1, 2) + M(2, 1);
//	N(3, 3) = -M(0, 0) - M(1, 1) + M(2, 2);
//
//
//	// Compute eigenvalues and eigenvectors (normalized, but unordered):
//	// "The eigenvectors are normalized to have (Euclidean) norm equal to one."
//	// "The eigenvalues are not sorted in any particular order."
//	// See: http://eigen.tuxfamily.org/dox/classEigen_1_1EigenSolver.html
//	Eigen::EigenSolver<Eigen::Matrix4d> es(N, true);
//
//	int maxEigenValueCol;
//	Eigen::Vector4cd ev_complex = es.eigenvalues();
//	Eigen::Vector4d ev = ev_complex.real();
//
//	if (ev(0) > ev(1) && ev(0) > ev(2) && ev(0) > ev(3)) maxEigenValueCol = 0;
//	else if (ev(1) > ev(2) && ev(1) > ev(3)) maxEigenValueCol = 1;
//	else if (ev(2) > ev(3)) maxEigenValueCol = 2;
//	else maxEigenValueCol = 3;
//
//	// Quaternion is maximum eigenvector
//	Eigen::Vector4cd q_complex = es.eigenvectors().col(maxEigenValueCol);
//	Eigen::Vector4d q = q_complex.real();
//
//	// Compute Rotation matrix
//	pose(0,0) = q(0)*q(0) + q(1)*q(1) - q(2)*q(2) - q(3)*q(3);
//	pose(0,1) = 2*(q(1)*q(2) - q(0)*q(3));
//	pose(0,2) = 2*(q(1)*q(3) + q(0)*q(2));
//
//	pose(1,0) = 2*(q(2)*q(1) + q(0)*q(3));
//	pose(1,1) = q(0)*q(0) - q(1)*q(1) + q(2)*q(2) - q(3)*q(3);
//	pose(1,2) = 2*(q(2)*q(3) - q(0)*q(1));
//
//	pose(2,0) = 2*(q(3)*q(1) - q(0)*q(2));
//	pose(2,1) = 2*(q(2)*q(3) + q(0)*q(1));
//	pose(2,2) = q(0)*q(0) - q(1)*q(1) - q(2)*q(2) + q(3)*q(3);
//
//	// Calculate translation: TMat = rightmeanMat - scale*RMat*leftmeanMat (scale=1)
//	pose.block<3,1>(0, 3) = rightMean - pose.block<3, 3>(0, 0) * leftMean;
//
//	pose(3, 3) = 1;
//
//	return pose;
//}

void FrameMatcher::setFeatureMatcher(FeatureMatcher & _featureMatcher) {
	featureMatcher = &_featureMatcher;
}

void FrameMatcher::setGlobalFeatureMap(GlobalFeatureMap & _globalFeatureMap) {
	globalFeatureMap = & _globalFeatureMap;
}
