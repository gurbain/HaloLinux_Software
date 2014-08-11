/*
 * AbsoluteOrientation.cpp
 *
 *  Created on: Jun 23, 2012
 *      Author: muggler
 */

#include "AbsoluteOrientation.h"
#include <iostream>

AbsoluteOrientation::AbsoluteOrientation() {
	// TODO Auto-generated constructor stub
}

// According to:
// Berthold K. P. Horn: Closed-form solution of absolute orientation using unit quaternions (1986)
// Implementation adapted from Brent Tweddle/Konrad Makowka (2011)
Eigen::Matrix4d AbsoluteOrientation::absoluteOrientation(std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > & left,
		std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > & right) {

	// set right dimensions of output matrix
	Eigen::Matrix4d pose = Eigen::Matrix4d::Zero();

	// compute the mean of the left and right set of points
	Eigen::Vector3d leftMean = Eigen::Vector3d::Zero();
	Eigen::Vector3d rightMean = Eigen::Vector3d::Zero();

	for (int i = 0; i < (int)left.size(); i++) {
		leftMean += left[i];
		rightMean += right[i];
	}

	leftMean /= left.size();
	rightMean /= right.size();

	// Move all points to the "center"
	for (int i = 0; i < (int)left.size(); i++) {
		left[i] -= leftMean;
		right[i] -= rightMean;
	}

	double scale_top = 0;
	double scale_bottom = 0;

	for (int i = 0; i < (int)left.size(); i++) {
		scale_top += right[i].squaredNorm();
		scale_bottom += left[i].squaredNorm();
	}
	double scale = sqrt(scale_top / scale_bottom);

//	std::cout << "scale: " << scale << std::endl;


//	//compute scale (use the symmetrical solution)
//	double Sl = 0;
//	double Sr = 0;
//
//	// this is the symmetrical version of the scale !
//	for (int i = 0; i < (int)left.size(); i++) {
//		Sl += left[i].transpose() * left[i]; 	//left[i].x*left[i].x + left[i].y*left[i].y + left[i].z*left[i].z;
//		Sr += right[i].transpose() * right[i]; 	//right[i].x*right[i].x + right[i].y*right[i].y + right[i].z*right[i].z;
//	}
//	double scale = Sr/Sl;

	//create M matrix
	Eigen::Matrix3d M = Eigen::Matrix3d::Zero();

	for (int i = 0; i < (int)left.size(); i++) {
		M(0, 0) += left[i](0) * right[i](0);
		M(0, 1) += left[i](0) * right[i](1);
		M(0, 2) += left[i](0) * right[i](2);
		M(1, 0) += left[i](1) * right[i](0);
		M(1, 1) += left[i](1) * right[i](1);
		M(1, 2) += left[i](1) * right[i](2);
		M(2, 0) += left[i](2) * right[i](0);
		M(2, 1) += left[i](2) * right[i](1);
		M(2, 2) += left[i](2) * right[i](2);
	}

	//create N matrix
	Eigen::Matrix4d N = Eigen::Matrix4d::Zero();
	N(0, 0) = M(0, 0) + M(1, 1) + M(2, 2);
	N(0, 1) = M(1, 2) - M(2, 1);
	N(0, 2) = M(2, 0) - M(0, 2);
	N(0, 3) = M(0, 1) - M(1, 0);

	N(1, 0) = M(1, 2) - M(2, 1);
	N(1, 1) = M(0, 0) - M(1, 1) - M(2, 2);
	N(1, 2) = M(0, 1) + M(1, 0);
	N(1, 3) = M(2, 0) + M(0, 2);

	N(2, 0) = M(2, 0) - M(0, 2);
	N(2, 1) = M(0, 1) + M(1, 0);
	N(2, 2) = -M(0, 0) + M(1, 1) - M(2, 2);
	N(2, 3) = M(1, 2) + M(2, 1);

	N(3, 0) = M(0, 1) - M(1, 0);
	N(3, 1) = M(2, 0) + M(0, 2);
	N(3, 2) = M(1, 2) + M(2, 1);
	N(3, 3) = -M(0, 0) - M(1, 1) + M(2, 2);


	// Compute eigenvalues and eigenvectors (normalized, but unordered):
	// "The eigenvectors are normalized to have (Euclidean) norm equal to one."
	// "The eigenvalues are not sorted in any particular order."
	// See: http://eigen.tuxfamily.org/dox/classEigen_1_1EigenSolver.html
	Eigen::EigenSolver<Eigen::Matrix4d> es(N, true);

	int maxEigenValueCol;
	Eigen::Vector4cd ev_complex = es.eigenvalues();
	Eigen::Vector4d ev = ev_complex.real();

	if (ev(0) > ev(1) && ev(0) > ev(2) && ev(0) > ev(3)) {
		maxEigenValueCol = 0;
	} else if (ev(1) > ev(2) && ev(1) > ev(3)) {
		maxEigenValueCol = 1;
	} else if (ev(2) > ev(3)) {
		maxEigenValueCol = 2;
	} else {
		maxEigenValueCol = 3;
	}

//	std::cout << "AbsOrient: ev: " << ev.transpose() << std::endl;
//	std::cout << "AbsOrient: maxEigenValueCol: " << maxEigenValueCol << std::endl;
	// Quaternion is maximum eigenvector
	Eigen::Vector4cd q_complex = es.eigenvectors().col(maxEigenValueCol);
	Eigen::Vector4d q = q_complex.real();
	q = q / q.norm();

//	std::cout << "AbsOrient: q: " << q.transpose() << std::endl;

/*
	//New code from Brent 11/27/2012 - see Sidi's textbook pg 324
	Eigen::Matrix3d rot;
	rot(0,0) = q(0)*q(0)-q(1)*q(1)-q(2)*q(2)+q(3)*q(3);
	rot(0,1) = 2*(q(0)*q(1)+q(2)*q(3));
	rot(0,2) = 2*(q(0)*q(2)-q(1)*q(3));

	rot(1,0) = 2*(q(0)*q(1)-q(2)*q(3));
	rot(1,1) = -q(0)*q(0)+q(1)*q(1)-q(2)*q(2)+q(3)*q(3);
	rot(1,2) = 2*(q(2)*q(1)+q(0)*q(3));

	rot(2,0) = 2*(q(0)*q(2)+q(1)*q(3));
	rot(2,1) = 2*(q(2)*q(1)-q(0)*q(3));
	rot(2,2) = -q(0)*q(0)-q(1)*q(1)+q(2)*q(2)+q(3)*q(3);
	pose.topLeftCorner(3,3) = rot.transpose();
*/
	//std::cout << "Rot: " << std::endl << rot << std::endl;

	//OLD CODE from Elias??

	// Compute Rotation matrix
	pose(0,0) = q(0)*q(0) + q(1)*q(1) - q(2)*q(2) - q(3)*q(3);
	pose(0,1) = 2*(q(1)*q(2) - q(0)*q(3));
	pose(0,2) = 2*(q(1)*q(3) + q(0)*q(2));

	pose(1,0) = 2*(q(2)*q(1) + q(0)*q(3));
	pose(1,1) = q(0)*q(0) - q(1)*q(1) + q(2)*q(2) - q(3)*q(3);
	pose(1,2) = 2*(q(2)*q(3) - q(0)*q(1));

	pose(2,0) = 2*(q(3)*q(1) - q(0)*q(2));
	pose(2,1) = 2*(q(2)*q(3) + q(0)*q(1));
	pose(2,2) = q(0)*q(0) - q(1)*q(1) - q(2)*q(2) + q(3)*q(3);

	// Calculate translation: TMat = rightmeanMat - scale*RMat*leftmeanMat (scale=1)
	pose.block<3,1>(0, 3) = rightMean - scale * pose.block<3, 3>(0, 0) * leftMean;

	pose(3, 3) = 1;
//	std::cout << "pose: " << std::endl << pose << std::endl;

/*	Eigen::Matrix4d pose2 = pose;
	Eigen::Vector3d T = pose.col(3).head(3);
	Eigen::Matrix3d R = pose.topLeftCorner(3,3);
	pose2.topLeftCorner(3,3) = R.transpose();
	pose2.col(3).head(3) = -R.transpose() * T;
*/
	return pose;

}
