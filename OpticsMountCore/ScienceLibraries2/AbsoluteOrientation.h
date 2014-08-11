/*
 * AbsoluteOrientation.h
 *
 *  Created on: Jun 23, 2012
 *      Author: muggler
 */

#ifndef ABSOLUTEORIENTATION_H_
#define ABSOLUTEORIENTATION_H_

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/Eigenvalues>
#include <Eigen/StdVector>

class AbsoluteOrientation {
public:
	AbsoluteOrientation();

	static Eigen::Matrix4d absoluteOrientation(std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > & left,
			std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > & right);

	// 32-bit issue
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif /* ABSOLUTEORIENTATION_H_ */
