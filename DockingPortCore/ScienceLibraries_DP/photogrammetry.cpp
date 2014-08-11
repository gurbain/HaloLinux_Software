#include "photogrammetry.h"


// ##################### triangulate two points #######################

//triangulates accross rectified image points (i.e. there is only a Tx separation between the x,y coords
//uses the method from Trucco and Verri " Introductory techniques for 3-D computer vision", pg 162
int Photogrammetry::triangulate(const cv::Point2f& leftFeaturePos, const cv::Point2f& rightFeaturePos, cv::Point3d & PXX) {
	cv::Point2d left_corners, right_corners;

	cv::Mat A = cv::Mat::zeros(3,3,CV_64F);
	cv::Mat X = cv::Mat::zeros(3,1,CV_64F);
	cv::Mat B = cv::Mat::zeros(3,1,CV_64F);

	cv::Mat pl = cv::Mat::zeros(3,1,CV_64F);
	cv::Mat pr = cv::Mat::zeros(3,1,CV_64F);
	cv::Mat w;
	cv::Mat pxx = cv::Mat::zeros(3,1,CV_64F);

	double a,b,c;

	left_corners.x = leftFeaturePos.x;
	left_corners.y = leftFeaturePos.y;
	right_corners.x = rightFeaturePos.x;
	right_corners.y = rightFeaturePos.y;

	left_corners.x -= cx_left;	//cx
	left_corners.y -= cy_left;	//cy
	right_corners.x -= cx_right;	//cx
	right_corners.y -= cy_right;	//cy

	pl.at<double>(0,0) = left_corners.x;
	pl.at<double>(1,0) = left_corners.y;
	pl.at<double>(2,0) = f_left;

	pr.at<double>(0,0) = right_corners.x;
	pr.at<double>(1,0) = right_corners.y;
	pr.at<double>(2,0) = f_right;

	w = pl.cross(pr);
/*
	std::cout << "pl: " << pl << endl;
	std::cout << "pr: " << pr << endl;
	std::cout << "w: " << w << endl;
*/
	A.at<double>(0,0) = pl.at<double>(0,0);
	A.at<double>(1,0) = pl.at<double>(1,0);
	A.at<double>(2,0) = pl.at<double>(2,0);

	A.at<double>(0,1) = -pr.at<double>(0,0);
	A.at<double>(1,1) = -pr.at<double>(1,0);
	A.at<double>(2,1) = -pr.at<double>(2,0);

	A.at<double>(0,2) = w.at<double>(0,0);
	A.at<double>(1,2) = w.at<double>(1,0);
	A.at<double>(2,2) = w.at<double>(2,0);

	B.at<double>(0,0) = -Tx; //distance between cameras
	B.at<double>(1,0) = -Ty;
	B.at<double>(2,0) = -Tz;

	solve(A, B, X); // Ax = B

	//std::cout <<endl << "A: " << A << endl << "B: "<< B << endl << "X: " << X << endl;

	a = X.at<double>(0,0);
	b = X.at<double>(1,0);
	c = X.at<double>(2,0);

	pxx = pixelSize * (a*pl + c/2 * w);

	PXX.x = pxx.at<double>(0,0);
	PXX.y = pxx.at<double>(1,0);
	PXX.z = pxx.at<double>(2,0);

//	std::cout << " PXX: " << PXX << endl;

//	std::cout << "cwp/2: " << c/2 * w<< endl;

	return 0;

}


// This is the model equation for the timeframe RANSAC
// B.K.P. Horn's closed form Absolute Orientation method (1987 paper)
// The convention used here is: right = (1/scale) * RMat * left + TMat
int Photogrammetry::absoluteOrientation(vector<cv::Point3d> & left, vector<cv::Point3d> & right, cv::Mat & RMat, cv::Mat & TMat, double & scale) {

	//check if both vectors have the same number of size
	if (left.size() != right.size()) {
		cerr << "Sizes don't match" << endl;
		return -1;
	}

	//compute the mean of the left and right set of points
	cv::Point3d leftmean, rightmean;

	leftmean.x = 0;
	leftmean.y = 0;
	leftmean.z = 0;
	rightmean.x = 0;
	rightmean.y = 0;
	rightmean.z = 0;

	for (int i = 0; i < left.size(); i++) {
		leftmean.x += left[i].x;
		leftmean.y += left[i].y;
		leftmean.z += left[i].z;

		rightmean.x += right[i].x;
		rightmean.y += right[i].y;
		rightmean.z += right[i].z;
	}

	leftmean.x /= left.size();
	leftmean.y /= left.size();
	leftmean.z /= left.size();

	rightmean.x /= right.size();
	rightmean.y /= right.size();
	rightmean.z /= right.size();

	cv::Mat leftmeanMat(3,1,CV_64F);
	cv::Mat rightmeanMat(3,1,CV_64F);

	leftmeanMat.at<double>(0,0) = leftmean.x;
	leftmeanMat.at<double>(0,1) = leftmean.y;
	leftmeanMat.at<double>(0,2) = leftmean.z;

	rightmeanMat.at<double>(0,0) = rightmean.x;
	rightmeanMat.at<double>(0,1) = rightmean.y;
	rightmeanMat.at<double>(0,2) = rightmean.z;

	//normalize all points
	for (int i = 0; i < left.size(); i++) {
		left[i].x -= leftmean.x;
		left[i].y -= leftmean.y;
		left[i].z -= leftmean.z;

		right[i].x -= rightmean.x;
		right[i].y -= rightmean.y;
		right[i].z -= rightmean.z;
	}

	//compute scale (use the symmetrical solution)
	double Sl = 0;
	double Sr = 0;

	// this is the symmetrical version of the scale !
	for (int i = 0; i < left.size(); i++) {
		Sl += left[i].x*left[i].x + left[i].y*left[i].y + left[i].z*left[i].z;
		Sr += right[i].x*right[i].x + right[i].y*right[i].y + right[i].z*right[i].z;
	}

	scale = sqrt(Sr/Sl);

//	cout << "Scale: " << scale << endl;


	//create M matrix
	double M[3][3];// = {0.0};
/*
		// I believe this is wrong, since not summing over all left right elements, just for the last element ! KM Nov 21
		for (int i = 0; i < left.size(); i++) {
			M[0][0] = left[i].x*right[i].x;
			M[0][1] = left[i].x*right[i].y;
			M[0][2] = left[i].x*right[i].z;
			M[1][0] = left[i].y*right[i].x;
			M[1][1] = left[i].y*right[i].y;
			M[1][2] = left[i].y*right[i].z;
			M[2][0] = left[i].z*right[i].x;
			M[2][1] = left[i].z*right[i].y;
			M[2][2] = left[i].z*right[i].z;
		}
*/
	M[0][0] = 0;
	M[0][1] = 0;
	M[0][2] = 0;
	M[1][0] = 0;
	M[1][1] = 0;
	M[1][2] = 0;
	M[2][0] = 0;
	M[2][1] = 0;
	M[2][2] = 0;

	for (int i = 0; i < left.size(); i++)
	{
		M[0][0] += left[i].x*right[i].x;
		M[0][1] += left[i].x*right[i].y;
		M[0][2] += left[i].x*right[i].z;
		M[1][0] += left[i].y*right[i].x;
		M[1][1] += left[i].y*right[i].y;
		M[1][2] += left[i].y*right[i].z;
		M[2][0] += left[i].z*right[i].x;
		M[2][1] += left[i].z*right[i].y;
		M[2][2] += left[i].z*right[i].z;
	}

	//create N matrix
	cv::Mat N = cv::Mat::zeros(4,4,CV_64F);

	N.at<double>(0,0) = M[0][0] + M[1][1] + M[2][2];
	N.at<double>(0,1) = M[1][2] - M[2][1];
	N.at<double>(0,2) = M[2][0] - M[0][2];
	N.at<double>(0,3) = M[0][1] - M[1][0];

	N.at<double>(1,0) = M[1][2] - M[2][1];
	N.at<double>(1,1) = M[0][0] - M[1][1] - M[2][2];
	N.at<double>(1,2) = M[0][1] + M[1][0];
	N.at<double>(1,3) = M[2][0] + M[0][2];

	N.at<double>(2,0) = M[2][0] - M[0][2];
	N.at<double>(2,1) = M[0][1] + M[1][0];
	N.at<double>(2,2) = -M[0][0] + M[1][1] - M[2][2];
	N.at<double>(2,3) = M[1][2] + M[2][1];

	N.at<double>(3,0) = M[0][1] - M[1][0];
	N.at<double>(3,1) = M[2][0] + M[0][2];
	N.at<double>(3,2) = M[1][2] + M[2][1];
	N.at<double>(3,3) = -M[0][0] - M[1][1] + M[2][2];

//	cout << "N: " << N << endl;

	//compute eigenvalues
	cv::Mat eigenvalues(1,4,CV_64FC1);
	cv::Mat eigenvectors(4,4,CV_64FC1);

//	cout << "eigenvalues: \n" << eigenvalues << endl;

	if (!cv::eigen(N, eigenvalues, eigenvectors)) {
		cerr << "eigen failed" << endl;
		return -1;
	}

//	cout << "Eigenvalues:\n" << eigenvalues << endl;
//	cout << "Eigenvectors:\n" << eigenvectors << endl;

	//compute quaterion as maximum eigenvector

	double q[4];
	q[0] = eigenvectors.at<double>(0,0);
	q[1] = eigenvectors.at<double>(0,1);
	q[2] = eigenvectors.at<double>(0,2);
	q[3] = eigenvectors.at<double>(0,3);

/*	// I believe this changed with the openCV implementation, eigenvectors are stored in row-order !
	q[0] = eigenvectors.at<double>(0,0);
	q[1] = eigenvectors.at<double>(1,0);
	q[2] = eigenvectors.at<double>(2,0);
	q[3] = eigenvectors.at<double>(3,0);
*/

	double absOfEigVec = sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
	q[0] /= absOfEigVec;
	q[1] /= absOfEigVec;
	q[2] /= absOfEigVec;
	q[3] /= absOfEigVec;

	cv::Mat qMat(4,1,CV_64F,q);
//	cout << "q: " << qMat << endl;

	//compute Rotation matrix

	RMat.at<double>(0,0) = q[0]*q[0] + q[1]*q[1] - q[2]*q[2] - q[3]*q[3];
	RMat.at<double>(0,1) = 2*(q[1]*q[2] - q[0]*q[3]);
	RMat.at<double>(0,2) = 2*(q[1]*q[3] + q[0]*q[2]);

	RMat.at<double>(1,0) = 2*(q[2]*q[1] + q[0]*q[3]);
	RMat.at<double>(1,1) = q[0]*q[0] - q[1]*q[1] + q[2]*q[2] - q[3]*q[3];
	RMat.at<double>(1,2) = 2*(q[2]*q[3] - q[0]*q[1]);

	RMat.at<double>(2,0) = 2*(q[3]*q[1] - q[0]*q[2]);
	RMat.at<double>(2,1) = 2*(q[2]*q[3] + q[0]*q[1]);
	RMat.at<double>(2,2) = q[0]*q[0] - q[1]*q[1] - q[2]*q[2] + q[3]*q[3];

//	cout <<"R:\n" << RMat << endl;
//	cout << "Det: " << determinant(RMat) << endl;

	//find translation
	cv::Mat tempMat(3,1,CV_64F);

	//gemm(RMat, leftmeanMat, -1.0, rightmeanMat, 1.0, TMat); // enforcing scale of 1, since same scales in both frames
	// The convention used here is: right = (1/scale) * RMat * left + TMat
	TMat = -(1/scale) * RMat*leftmeanMat + rightmeanMat;

	//	gemm(RMat, leftmeanMat, -1.0 * scale, rightmeanMat, 1.0, TMat);

//	cout << "Translation: " << TMat << endl;
	return 0;
}

/* Haralick's Globally Convergent Algorithm. Described in:
 * 		Haralick et. al., "Pose Estimation from Corresponding Point Data," IEEE Trans on Systems, Man and Cybernetics, Vol 16, No 6, 1987
 * 		and
 * 		Tweddle, "Relative Computer Vision Based Navigation for Small Inspection Spacecraft," AIAA GNC 2011
 */
int Photogrammetry::exteriorOrientation(vector<cv::Point2f> & imgPoints, vector<cv::Point3d> & objPoints, cv::Mat & RMat, cv::Mat & TMat,
		vector<double> & d, double & meanSquaredError,
		bool usingLeft=true, int maxIterations=1000, double errorThreshold=1.0e-10) {

	int iterations = 0;
	double scale, diffError;
	cv::Mat reProjPoint;

	cv::Point3d pointError;
	double totalError, prevError;

	//unit direction vector
	vector<cv::Point3d> v;

	//scaled direction vector to points
	vector<cv::Point3d> dv;
	double f = usingLeft ? f_left : f_right;

	if (imgPoints.size() != objPoints.size() || imgPoints.size() != d.size()) {
		return -1;
	}

	center2dPoints(imgPoints, usingLeft);

	int n = imgPoints.size();

	//create v and d
	for (int i=0; i < n; i++ ) {
		v.push_back(cv::Point3d(imgPoints[i].x / f, imgPoints[i].y / f, 1.0));
	}

	for (int i=0; i < n; i++ ) {
		dv.push_back(cv::Point3d(d[i] * v[i].x, d[i] * v[i].y, d[i] * v[i].z));
	}

	meanSquaredError = 100*n; //large initial error
	do {
		prevError = meanSquaredError;
		totalError = 0;

		//solve absolute orientation
		this->absoluteOrientation(objPoints, dv, RMat, TMat, scale);


		//determine new d and compute mean squared error
		for (int i=0; i < n; i++) {
			reProjPoint = (1/scale) * RMat * cv::Mat(objPoints[i]) + TMat;

			d[i] = reProjPoint.dot(cv::Mat(v[i])) / (v[i].dot(v[i]));

			dv[i] = d[i] * v[i];
			pointError = cv::Point3d(reProjPoint) - dv[i];
			totalError += sqrt(pointError.dot(pointError));
		}
		meanSquaredError = totalError / n;

		iterations++;
		diffError = prevError - meanSquaredError;
	} while(iterations < maxIterations && ( abs(diffError) > errorThreshold));

//	cout << "Iterations: " << iterations << endl;
//	cout << "Diff Error: " << prevError - meanSquaredError << endl;

	return 0;
}

void Photogrammetry::center2dPoints(vector<cv::Point2f> & imgPoints, bool left) {

	for (int i=0; i< imgPoints.size(); i++) {
		imgPoints[i].x -= left ? cx_left : cx_right;
		imgPoints[i].y -= left ? cy_left : cy_right;
	}
}
