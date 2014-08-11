#include "additionalGuestscientistCode.h"

using namespace std;
using namespace cv;


void GuestScientistClass::extractFeatures(Mat& leftImage, Mat& rightImage) {

//	FAST(leftImage, this->leftFeatures, this->FASTthresh, true);
//	FAST(rightImage, this->rightFeatures, this->FASTthresh, true);


	SURF surfDetector(1500, 4, 2, false, true);
	Mat zeroMask;
	surfDetector(leftImage, zeroMask, this->leftFeatures);
	surfDetector(rightImage, zeroMask, this->rightFeatures);

/*
	Mat zeroMask;
	siftDetector(leftImage, zeroMask, this->leftFeatures);
	siftDetector(rightImage, zeroMask, this->rightFeatures);
*/

}

void GuestScientistClass::extractFeatureDescriptors(Mat& leftImage,
		Mat& rightImage) {

	SurfDescriptorExtractor surfDescriptorExtractor(4, 2, false);
	surfDescriptorExtractor.compute(leftImage, this->leftFeatures, this->leftDescriptors);
	surfDescriptorExtractor.compute(rightImage, this->rightFeatures, this->rightDescriptors);

/*
	siftDescriptorExtractor.compute(leftImage, this->leftFeatures, this->leftDescriptors);
	siftDescriptorExtractor.compute(rightImage, this->rightFeatures, this->rightDescriptors);
*/

}

void GuestScientistClass::stereoMatchFeatures(Mat& leftImage, Mat& rightImage, int& numberMatches) {

	this->stereomatcher.initialStereoMatch(this->leftFeatures, this->rightFeatures, this->leftDescriptors, this->rightDescriptors, this->stereoFeatureMatches, this->leftFeaturesPreMatch, this->rightFeaturesPreMatch);

	// this is the Fundamental Matrix RANSAC version
	if (this->stereoFeatureMatches.size() > 8) // RANSAC needs at least 8 matches, and we need more than 8 for RANSAC to make sense
	{
		this->stereomatcher.getStereoInliersRANSAC_Fundamental(this->leftFeatures, this->rightFeatures, this->leftFeaturesRANSAC,
				this->rightFeaturesRANSAC, this->leftDescriptors, this->rightDescriptors, this->stereoFeatureMatches,
				this->stereoFeatureMatchesRANSAC);
	}

	numberMatches = this->stereoFeatureMatchesRANSAC.size();

//	printf("STEREO: numOf initial: %d \t numOf RANSAC: %d\n", this->leftFeaturesPreMatch.size(), this->stereoFeatureMatchesRANSAC.size());

/*
	// this is the Homography Matrix RANSAC version
	if (this->stereoFeatureMatches.size() > 5) // RANSAC needs at least 8 matches, and we need more than 8 for RANSAC to make sense
	{
		this->stereomatcher.getStereoInliersRANSAC_Homography(this->leftFeatures, this->rightFeatures, this->leftFeaturesRANSAC,
				this->rightFeaturesRANSAC, this->leftDescriptors, this->rightDescriptors, this->stereoFeatureMatches,
				this->stereoFeatureMatchesRANSAC);
	}
*/

/*
	// this is the Mutliple Homography Matrix RANSAC version
	if (this->stereoFeatureMatches.size() > 5) // RANSAC needs at least 8 matches, and we need more than 8 for RANSAC to make sense
	{
		this->stereomatcher.getStereoInliersRANSAC_MultiHomography(this->leftFeatures, this->rightFeatures, this->leftFeaturesRANSAC,
				this->rightFeaturesRANSAC, this->leftDescriptors, this->rightDescriptors, this->stereoFeatureMatches,
				this->stereoFeatureMatchesRANSAC);
	}
*/

}

void GuestScientistClass::showStereoMatches(Mat& leftImage, Mat& rightImage, Mat& matchesImage) {

	drawMatches(leftImage, this->leftFeatures, rightImage, this->rightFeatures,	this->stereoFeatureMatchesRANSAC, matchesImage, 255, Scalar::all(-1));
//	drawMatches(leftImage, this->leftFeatures, rightImage, this->rightFeatures, this->stereoFeatureMatches, matchesImage, 255, Scalar::all(-1)); // this is just to see, what initialStereoMatching does
}

int GuestScientistClass::triangulateChessboard(vector<Point2f>& leftImgPoints, vector<Point2f>& rightImgPoints,
		int chessboardWidth, int chessboardHeight, double & roll, double & pitch, double & yaw, Mat & Tout,
		double mean_squares[], double stddev_squares[]) {
	int index;
	vector<Point3d> measuredPoints;
	vector<Point3d> objectPoints;
	Point3d xyzPoint;
	Point3d p1, p2;
	double squareSize = 0.0254;
	Mat R, T;
	double scale;

	R.create(3, 3, CV_64FC1);
	T.create(3, 1, CV_64FC1);

	if (leftImgPoints.size() == chessboardWidth*chessboardHeight && rightImgPoints.size() == chessboardWidth*chessboardHeight) {
		for (int y = 0; y < chessboardHeight; y++) {
			for (int x = 0; x < chessboardWidth; x++) {
				index = x+y*chessboardWidth;

				//std::cout << "left: " << leftImgPoints[index] << " right: " << rightImgPoints[index];
				this->photo->triangulate(leftImgPoints[index], rightImgPoints[index], xyzPoint);
				//std::cout << "(" << x << "," << y << "): " << xyzPoint << endl;

				//cout << "XYZ Point: " << xyzPoint << endl;

				measuredPoints.push_back(xyzPoint);
				objectPoints.push_back(Point3d((x - (int) floor(chessboardWidth/2))*squareSize, (y - (int) floor(chessboardHeight/2))*squareSize, 0));
			}
		}

//		cout << endl;

		this->photo->absoluteOrientation(objectPoints, measuredPoints, R, T, scale);
//		std::cout << "R: " << endl << R << endl << "T: " << endl << T << endl << "Scale: " << scale << endl;

		yaw = atan(R.at<double>(0,1)/R.at<double>(1,1))*180 / M_PI;
		pitch = -asin(R.at<double>(2,1))*180 / M_PI;
		roll = atan(R.at<double>(2,0)/R.at<double>(2,2))*180 / M_PI;

		Tout = T;

		//compute statistics
		//X DIRECTION
		Mat p1obj, p2obj, d3;
		vector<double> dvect;
		double d;
		double mean_d = 0;
		double stddev_d = 0;
		int n = 0;
		for (int y = 0; y < chessboardHeight-1; y++) {
			for (int x = 0; x < chessboardWidth-1; x++) {
				p1 = measuredPoints[x+y*chessboardWidth];
				p2 = measuredPoints[(x+1)+y*chessboardWidth];

				//gemm(R, Mat(p1), 1/scale, NULL, 0, p1obj, GEMM_1_T);
				p1obj = (1/scale) * R * Mat(p1);
				//gemm(R, Mat(p2), 1/scale, NULL, 0, p2obj, GEMM_1_T);
				p2obj = (1/scale) * R * Mat(p2);

				subtract(p1obj, p2obj, d3);
				d = norm(d3);
				dvect.push_back(d);
				mean_d += d;

				n++;
			}
		}
		mean_d /= n;

		vector<double>::iterator it;
		for (it = dvect.begin(); it < dvect.end(); it++) {
			d = *it;
			stddev_d += (d - mean_d)*(d - mean_d);
		}
		stddev_d = sqrt(stddev_d / (n-1));

		stddev_squares[0] = stddev_d;
		mean_squares[0] = mean_d;

		//Y DIRECTION
		dvect.clear();
		mean_d = 0;
		stddev_d = 0;
		n = 0;
		for (int y = 0; y < chessboardHeight-1; y++) {
			for (int x = 0; x < chessboardWidth-1; x++) {
				p1 = measuredPoints[x+y*chessboardWidth];
				p2 = measuredPoints[x+(y+1)*chessboardWidth];

				//gemm(R, Mat(p1), 1/scale, NULL, 0, p1obj, GEMM_1_T);
				p1obj = (1/scale) * R * Mat(p1);
				//gemm(R, Mat(p2), 1/scale, NULL, 0, p2obj, GEMM_1_T);
				p2obj = (1/scale) * R * Mat(p2);

				subtract(p1obj, p2obj, d3);
				d = norm(d3);
				dvect.push_back(d);
				mean_d += d;

				n++;
			}
		}
		mean_d /= n;

		for (it = dvect.begin(); it < dvect.end(); it++) {
			d = *it;
			stddev_d += (d - mean_d)*(d - mean_d);
		}
		stddev_d = sqrt(stddev_d / (n-1));

		stddev_squares[1] = stddev_d;
		mean_squares[1] = mean_d;


	} else {
		roll = 0;
		pitch = 0;
		yaw = 0;

		Tout = T;

		return -1;
	}

	return 0;
}

int GuestScientistClass::monocularChessboard(vector<Point2f>& imgPoints,
		int chessboardWidth, int chessboardHeight, bool usingLeft, double & roll, double & pitch, double & yaw, Mat & Tout, double & mse) {
	int index;
	vector<Point3d> objectPoints;
	double squareSize = 0.0254;
	Mat R, T;

	R.create(3, 3, CV_64FC1);
	T.create(3, 1, CV_64FC1);

	if (imgPoints.size() == chessboardWidth*chessboardHeight) {
		for (int y = 0; y < chessboardHeight; y++) {
			for (int x = 0; x < chessboardWidth; x++) {
				index = x+y*chessboardWidth;

				objectPoints.push_back(Point3d((x - (int) floor(chessboardWidth/2))*squareSize, (y - (int) floor(chessboardHeight/2))*squareSize, 0));

				if (dval_init == false) {
					dval.push_back(0.5);
				}
			}
		}

		dval_init = true;

		this->photo->exteriorOrientation(imgPoints, objectPoints, R, T, dval, mse, usingLeft, 1000, 1.0e-10);

		yaw = atan(R.at<double>(0,1)/R.at<double>(1,1))*180 / M_PI;
		pitch = -asin(R.at<double>(2,1))*180 / M_PI;
		roll = atan(R.at<double>(2,0)/R.at<double>(2,2))*180 / M_PI;

		Tout = T;
	} else {
		roll = 0;
		pitch = 0;
		yaw = 0;

		Tout = T;

		return -1;
	}

	return 0;
}

