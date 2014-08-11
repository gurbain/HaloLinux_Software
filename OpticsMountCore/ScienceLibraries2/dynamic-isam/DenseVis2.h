#ifndef DENSEVIS2_H_
#define DENSEVIS2_H_

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <exception>

// Eigen
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <Eigen/StdVector>

// OpenCV
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"

// libelas
#include "elas.h"

#include "../FrameCatalogue.h"
#include "../Triangulator.h"
#include "../SimpleTriangulator.h"
#include "../DenseStereo.h"


//isam
#include "dynamicPose3d_NL.h"
#include "slam_dynamic3d_NL.h"
#include "rigidBodyDynamics.h"
#include <isam/robust.h>
#include <Eigen/Dense>


class DenseVis2 {
	cv::Mat elasDisp, nonthresholded_img;
	DenseStereo* denseStereo;

	 std::vector<Eigen::Vector3d, Eigen::aligned_allocator<Eigen::Vector3d> > princAxisPoints;
	 std::vector<int> princAxisColors;

	 Eigen::Vector3d currPoint; int currColor;

	std::ofstream densePoints_out;

	std::vector<Eigen::Vector3d,Eigen::aligned_allocator<Eigen::Vector3d> > densePoints;
	std::vector<int> denseGrayColors;

public:
	 EIGEN_MAKE_ALIGNED_OPERATOR_NEW

	DenseVis2(Triangulator* triangulator);

	void computeDensePoints(isam::cameraPose3d_Node* cam, isam::dynamicPose3d_NL_Node* pose, cv::Mat& leftImage, cv::Mat& rightImage);
	void buildDenseMap(isam::cameraPose3d_Node* cam, isam::dynamicPose3d_NL_Node* princAxis,
			std::vector<isam::dynamicPose3d_NL_Node*>& poselist, FrameCatalogue fc, std::string& folder);

};


#endif
