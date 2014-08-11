/*
 * SimpleTriangulator.h
 *
 *  Created on: Feb 28, 2012
 *      Author: muggler
 */

#ifndef SIMPLETRIANGULATOR_H_
#define SIMPLETRIANGULATOR_H_

#include "opencv2/core/core.hpp"

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>

#include "Triangulator.h"
#include "Camera.h"
//#include "Frame.h"

class SimpleTriangulator : public Triangulator {
public:
	SimpleTriangulator(Camera & camera) : Triangulator(camera) {}

	virtual Eigen::Vector3d triangulate(double u1, double u2, double v) {
		double z = (camera->tx * camera->f) / (u1-u2);
		double x = (u1 - camera->cx) * z / camera->f;
		double y = (v - camera->cy) * z / camera->f;
		return Eigen::Vector3d(x, y, z);
	}

	virtual Eigen::Vector3d triangulate(const cv::Point2d & p1, const cv::Point2d & p2) {
		return triangulate(p1.x, p2.x, p1.y);
	}

	virtual cv::Point2d reproject(double x, double y, double z) {
		cv::Point2d p;
		p.x = (x * camera->f / z) + camera->cx; //u1
		p.y = (y * camera->f / z) + camera->cy; //v
		return p;
	}

	virtual cv::Point2d reproject(const Eigen::Vector3d & p) {
		return reproject(p(0), p(1), p(2));
	}
};

#endif /* SIMPLETRIANGULATOR_H_ */
