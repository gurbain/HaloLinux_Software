/*
 * GlobalFeatureMap.h
 *
 *  Created on: Mar 25, 2012
 *      Author: muggler
 */

#ifndef GLOBALFEATUREMAP_H_
#define GLOBALFEATUREMAP_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

class GlobalFeatureMap {
public:
	GlobalFeatureMap();

	void save(const std::string & filename);
	void load(const std::string & filename);

	void saveAsPCD(const std::string & filename);

	// 32-bit issue
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > points;
	cv::Mat descriptors;
};

#endif /* GLOBALFEATUREMAP_H_ */
