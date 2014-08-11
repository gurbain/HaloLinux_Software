/*
 * DenseStereo.cpp
 *
 *  Created on: Apr 2, 2012
 *      Author: muggler
 */

#include "DenseStereo.h"

DenseStereo::DenseStereo(Triangulator & _triangulator, bool _subsample) {
	triangulator = &_triangulator;
	subsample = _subsample;

	// Block matching settings depending on disparity range
	minDisparity = triangulator->minDisparity >= 0 ? triangulator->minDisparity : 0;
	maxDisparity = triangulator->maxDisparity <=255 ? triangulator->maxDisparity : 255;

	// Initialize with presets
//	pixelStep = 1;

	int w, h;
	if (subsample) {
		w = triangulator->getCamera().width / 2;
		h = triangulator->getCamera().height / 2;
	} else {
		w = triangulator->getCamera().width;
		h = triangulator->getCamera().height;
	}

	// Allocate memory for matching
	D1_data = (float*)malloc(w*h*sizeof(float));
	D2_data = (float*)malloc(w*h*sizeof(float));
}

cv::Mat DenseStereo::getDisparityImage(const cv::Mat & leftImage, const cv::Mat & rightImage) {
	//cv::Mat disparityImage(leftImage.size(), CV_16S);


	// dimensions
	int32_t dims[3];
	if (subsample) {
		dims[0] = leftImage.cols;
		dims[1] = leftImage.rows;
		dims[2] = leftImage.cols;
	} else {
		dims[0] = leftImage.cols;
		dims[1] = leftImage.rows;
		dims[2] = leftImage.cols;
	}
	// library initialization
	Elas::parameters param(Elas::ROBOTICS);
	param.subsampling = subsample;
//	param.disp_min = 10;
//	param.disp_max = 255;
//	param.disp_min = minDisparity;
//	param.disp_max = maxDisparity;


//	param.match_texture = 10;
//	param.ipol_gap_width = 100;
	//param.support_threshold = 0.95;
	//param.incon_min_support = 30;
	//param.candidate_stepsize = 3;
	//param.grid_size = 5;
//	param.beta = 0.02; // image likelihood parameter	(0.02,0.02
//	param.gamma = 3; // prior constant	(3,5)
//	param.sigma = 1; // prior sigma		(1,1)
//	param.sradius = 2; // prior sigma radius	(2,3)


//	std::cout << "Disparity Min, Max: [" << param.disp_min << ", " << param.disp_max << "]\n";

	Elas elas(param);

	// dense matching
	elas.process((uint8_t*) leftImage.data, (uint8_t*) rightImage.data, D1_data, D2_data, dims);

	if (elas.getExceptionOccurred()) {
		throw DenseStereoException();
	}
	cv::Mat returnImage;

	//std::cout << "min/max disp: " << minDisparity << "," << maxDisparity << std::endl;
	// copy back to disparity image
	if (subsample) {
		cv::Mat disparityImage_small;
		disparityImage_float = cv::Mat(cv::Size(dims[0]/2,dims[1]/2),CV_32F,D1_data,cv::Mat::AUTO_STEP);
		disparityImage_float.convertTo(disparityImage_small, CV_8U,1.0, 0.0);
		//cv::resize(disparityImage_small, disparityImage,cv::Size(2.0*disparityImage_small.cols, 2.0*disparityImage_small.rows),0, 0, cv::INTER_NEAREST);
		disparityImage_small.copyTo(nonthresh_disp);
		cv::threshold(disparityImage_float, disparityImage_float, minDisparity, maxDisparity,cv::THRESH_TOZERO);
		cv::threshold(disparityImage_small, returnImage, minDisparity, maxDisparity,cv::THRESH_TOZERO);

		return returnImage;
	} else {
		disparityImage_float = cv::Mat(cv::Size(dims[0],dims[1]),CV_32F,D1_data,cv::Mat::AUTO_STEP);
		disparityImage_float.convertTo(disparityImage, CV_8U,1.0, 0.0);
		disparityImage.copyTo(nonthresh_disp);

		cv::threshold(disparityImage_float, disparityImage_float, minDisparity, maxDisparity,cv::THRESH_TOZERO);
		cv::threshold(disparityImage, returnImage, minDisparity, maxDisparity,cv::THRESH_TOZERO);
		return returnImage;
	}

}


cv::Mat DenseStereo::calculate(const cv::Mat & leftImage, const cv::Mat & rightImage) {
	// Get disparity map
	cv::Mat disparityImage = getDisparityImage(leftImage, rightImage);

	Eigen::Vector3d total;
	double numberPoints = 0;

	// Triangulate points
	for (int i=0; i<disparityImage_float.cols; i++) {
		for (int j=0; j<disparityImage_float.rows; j++) {
			// Get disparity and convert to pixel scale
			double d = disparityImage_float.at<float>(j, i);

			// Add point only if with disparity range
			if (d > minDisparity && d < maxDisparity) {
				Eigen::Vector4d ph, ph2;
				Eigen::Vector3d p, pa;

				// Triangulate point
				if (subsample) {
					p = triangulator->triangulate(i*2, i*2 - d, j*2);
				} else {
					p = triangulator->triangulate(i, i-d, j);
				}

				//pa << p(2), -p(0), -p(1);

				total += p;
				numberPoints++;

				// Store point
				points.push_back( p);

				if (subsample) {
					colors.push_back(leftImage.at<uint8_t>(j*2, i*2));
				} else{
					colors.push_back(leftImage.at<uint8_t>(j, i));
				}

			}
		}
	}

//	std::cout << "Points size: " << points.size() << std::endl;

	meanPoints = total / numberPoints;

	return disparityImage;
}


void DenseStereo::save(const std::string & filename, bool withColor) {
	// Open file
	std::ofstream file;
	file.open (filename.c_str());

	// Write header: http://pointclouds.org/documentation/tutorials/pcd_file_format.php
	file << "# .PCD v.5 - Point Cloud Data file format" << std::endl;
	if (withColor) {
		file << "FIELDS x y z intensity" << std::endl;
		file << "SIZE 4 4 4 4" << std::endl;
		file << "TYPE F F F F" << std::endl;
	} else {
		file << "FIELDS x y z" << std::endl;
		file << "SIZE 4 4 4" << std::endl;
		file << "TYPE F F F" << std::endl;
	}
	file << "WIDTH " << points.size() << std::endl;
	file << "HEIGHT 1" << std::endl;
	file << "POINTS " << points.size() << std::endl;
	file << "DATA ascii" << std::endl;

	for (int i=0; i < (int)points.size(); i++) {
		file << points[i](0) << " ";
		file << points[i](1) << " ";
		file << points[i](2);
		if (withColor) {
			file << " " << colors[i];
		}
		file << std::endl;
	}

	file.close();
}

void DenseStereo::clear() {
	points.clear();
	colors.clear();
}

int DenseStereo::getNumberOfPoints() {
	return points.size();
}

Eigen::Vector3d DenseStereo::getPoint(int i) {
	return points[i];
}
