#include "DenseVis2.h"

DenseVis2::DenseVis2(Triangulator* triangulator) {
	denseStereo = new DenseStereo(*triangulator, false);
}

void DenseVis2::buildDenseMap(isam::cameraPose3d_Node* cam, isam::dynamicPose3d_NL_Node* princAxis,
		std::vector<isam::dynamicPose3d_NL_Node*>& poselist, FrameCatalogue fc, std::string& folder) {
	Frame* frame;

	densePoints.clear();
	denseGrayColors.clear();

	int size = poselist.size();
	for (int i = 0; i < size ; i++) {
		std::cout << "Dense Map Iteration: " << i << std::endl;
		frame = fc.getFrame(i);
		computeDensePoints(cam, poselist[i], frame->leftImage, frame->rightImage);
	}

	densePoints_out.open((folder + "densePointsPLY.ply").c_str());
	densePoints_out << "ply" << std::endl;
	densePoints_out << "format ascii 1.0" << std::endl;
	densePoints_out << "comment author: Brent Tweddle" << std::endl;
	densePoints_out << "element vertex " << densePoints.size() << std::endl;
	densePoints_out << "property float x" << std::endl;
	densePoints_out << "property float y" << std::endl;
	densePoints_out << "property float z" << std::endl;
	densePoints_out << "property uchar red" << std::endl;
	densePoints_out << "property uchar green" << std::endl;
	densePoints_out << "property uchar blue" << std::endl;
	densePoints_out << "end_header" << std::endl;

	for (int i = 0; i < densePoints.size(); i++) {
		Eigen::Vector3d cPt = densePoints[i];
		int cColor = denseGrayColors[i];
		densePoints_out << cPt[0] << " " << cPt[1] << " " << cPt[2] << " " << cColor << " " << cColor << " " << cColor << " " << std::endl;
	}

	densePoints_out.close();

}

void DenseVis2::computeDensePoints(isam::cameraPose3d_Node* cam, isam::dynamicPose3d_NL_Node* pose, cv::Mat& leftImage, cv::Mat& rightImage) {
	 double lx, ly, lz;
	 isam::Point3dh X;
	 isam::Point3dh inertX;
	 isam::Point3dh bodyX;
	 Eigen::Vector3d bodyVec;
	 //cv::equalizeHist(leftImage, leftImage);
	 //cv::equalizeHist(rightImage, rightImage);

	 denseStereo->clear();

	 elasDisp = denseStereo->calculate(leftImage, rightImage);
	 nonthresholded_img = denseStereo->getPreThreshDisp();

	 for (unsigned int i = 0; i < denseStereo->points.size(); i++) {
		 lx = denseStereo->points[i](0);
		 ly = denseStereo->points[i](1);
		 lz = denseStereo->points[i](2);

		 X.set(lz, -lx, -ly, 1.0);
		 inertX = cam->value().transform_from(X);
		 bodyX = pose->value().transform_to_body(inertX);

		 currPoint = Eigen::Vector3d(bodyX.x(), bodyX.y(), bodyX.z());
		 currColor = denseStereo->colors[i];

		 densePoints.push_back(currPoint);
		 denseGrayColors.push_back(currColor);

		 densePoints_out << currPoint.x() << " " << currPoint.y() << " " << currPoint.z() << std::endl;
//		 princAxisPoints.push_back(;
//		 princAxisColors.push_back(denseStereo->colors[i]);

//		 std::cout << "Iteration: " << i << ", ";
//		 std::cout << "Num points: " << princAxisPoints.size() << std::endl;
	 }

	 //std::cout << "princAxis Point: " << princAxisPoints[1].x << "," << princAxisPoints[2].x << "," << princAxisPoints[3].x << "," << princAxisColors[1] << std::endl;

/*
	 stringstream filename2, filename1;
	 filename1 << "/home/tweddle/Desktop/elasDisp.bmp";
	 cv::imwrite(filename1.str(), elasDisp);
	 filename2 << "/home/tweddle/Desktop/nonthresholded_img.bmp";
	 cv::imwrite(filename2.str(), nonthresholded_img);
*/
}
