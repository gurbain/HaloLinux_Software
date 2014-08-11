/*
 * GlobalFeatureMap.cpp
 *
 *  Created on: Mar 25, 2012
 *      Author: muggler
 */

#include "GlobalFeatureMap.h"

GlobalFeatureMap::GlobalFeatureMap() {
	// TODO Auto-generated constructor stub
}

void GlobalFeatureMap::load(const std::string & filename) {
	cv::FileStorage fs(filename, cv::FileStorage::READ);

	int descriptorLength = fs["descriptor_length"];
	int descriptor_type = fs["descriptor_type"];

	cv::FileNode features = fs["descriptors"];
	cv::FileNodeIterator it = features.begin(), it_end = features.end();
	int idx = 0;

	Eigen::Vector3d* point;
	cv::Mat* descriptor;

	int i=0;

	points.clear();
	descriptors = cv::Mat(features.size(), descriptorLength, descriptor_type);

	for( ; it != it_end; ++it, idx++ ) {
		point = new Eigen::Vector3d( (double)(*it)["x"], (double)(*it)["y"], (double)(*it)["z"] );
		points.push_back(*point);

		//Mat desc;
		descriptor = new cv::Mat;
		(*it)["descriptor"] >> *descriptor;

		cv::Mat temp = descriptors.row(i);
		descriptor->copyTo(temp);
		i++;
	}
	fs.release();
}

void GlobalFeatureMap::save(const std::string & filename) {
	cv::FileStorage fs(filename, cv::FileStorage::WRITE);

	fs << "descriptor_type" <<  descriptors.row(0).type();
	fs << "descriptor_length" <<  descriptors.cols;

	fs << "descriptors" << "[";
	for (int i=0; i < (int)points.size(); i++) {
			fs << "{";
			fs << "x" << points[i](0);
			fs << "y" << points[i](1);
			fs << "z" << points[i](2);
			fs << "descriptor" <<  descriptors.row(i);
			fs << "}";
	}
	fs << "]";

	// Close file
	fs.release();
}

void GlobalFeatureMap::saveAsPCD(const std::string & filename) {
	// Open file
	std::ofstream file;
	file.open (filename.c_str());

	// Write header
	file << "# .PCD v.5 - Point Cloud Data file format" << std::endl;
	file << "FIELDS x y z" << std::endl;
	file << "SIZE 4 4 4" << std::endl;
	file << "TYPE F F F" << std::endl;
	file << "WIDTH " << points.size() << std::endl;
	file << "HEIGHT 1" << std::endl;
	file << "POINTS " << points.size() << std::endl;
	file << "DATA ascii" << std::endl;

	for (int i=0; i < (int)points.size(); i++) {
		file << points[i](0) << " ";
		file << points[i](1) << " ";
		file << points[i](2) << std::endl;
	}

	file.close();
}
