/*
 * Triangulator.h
 *
 *  Created on: Mar 23, 2012
 *      Author: muggler
 */

#ifndef TRIANGULATOR_H_
#define TRIANGULATOR_H_

#include "opencv2/core/core.hpp"

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>

#include "Camera.h"

class Triangulator {
public:
	Triangulator(Camera & _camera);

	// TODO: Generalize with v1 AND v2
	virtual Eigen::Vector3d triangulate(double u1, double u2, double v) = 0;
	virtual Eigen::Vector3d triangulate(const cv::Point2d & p1, const cv::Point2d & p2) = 0;

	virtual cv::Point2d reproject(double x, double y, double z) = 0;
	virtual cv::Point2d reproject(const Eigen::Vector3d & p) = 0;

	// 32-bit issue
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	double maxVerticalOffset;
	double minDisparity, maxDisparity;

	Camera getCamera() { return *camera; };

protected:
	Camera* camera;
};

#endif /* TRIANGULATOR_H_ */

