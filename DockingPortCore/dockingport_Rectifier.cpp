#include "dockingport_Rectifier.h"



int dockingport_Rectifier::calcRectificationMaps(int imgwidth, int imgheight, const char calibParamDir[200])
{
	// file names and paths to intrinsics and extrinsics
	// files are read by calcRectificationMaps
	sprintf(intrinsic_filename, "%s/intrinsics.yml", calibParamDir);
	sprintf(extrinsic_filename, "%s/extrinsics.yml", calibParamDir);

	cout << "Opening files: " << intrinsic_filename << ", " << extrinsic_filename << endl;

    //read intrinsics and extrinsics
	cv::FileStorage fs(intrinsic_filename, CV_STORAGE_READ);
    if(!fs.isOpened())
    {
        printf("Failed to open file %s\n", intrinsic_filename);
        cout << "Check camera calibration parameter directory environment variable and correct calibration set number" << endl;
        return -1;
    }

    fs["M1"] >> M1;
    fs["D1"] >> D1;
    fs["M2"] >> M2;
    fs["D2"] >> D2;

    fs.open(extrinsic_filename, CV_STORAGE_READ);
    if(!fs.isOpened())
    {
        printf("Failed to open file %s\n", extrinsic_filename);
        return -1;
    }

    fs["R"] >> R;
    fs["T"] >> T;

    //compute rectification matriciesC
    stereoRectify( M1, D1, M2, D2, cv::Size(imgwidth, imgheight), R, T, R1, R2, P1, P2, Q, CV_CALIB_ZERO_DISPARITY,
    		0, cv::Size(640,480), &roi1, &roi2 );

    //compute rect maps
    cv::initUndistortRectifyMap(M1, D1, R1, P1, cv::Size(imgwidth, imgheight), CV_16SC2, this->LeftRectMap1, this->LeftRectMap2);
    cv::initUndistortRectifyMap(M2, D2, R2, P2, cv::Size(imgwidth, imgheight), CV_16SC2, this->RightRectMap1, this->RightRectMap2);

	f = P1.at<double>(0,0);
	Tx = T.at<double>(0,0)*0.0254/(6*1.0e-6);
	Ty = T.at<double>(0,1)*0.0254/(6*1.0e-6);
	Tz = T.at<double>(0,2)*0.0254/(6*1.0e-6);
	cx = P2.at<double>(0,2);
	cy = P2.at<double>(1,2);

	return 0;
}


int dockingport_Rectifier::rectifyImages(cv::Mat& leftImgFrame, cv::Mat& rightImgFrame)
{

	cv::Mat leftDummyImage;
	cv::remap(leftImgFrame, leftDummyImage, this->LeftRectMap1, this->LeftRectMap2, CV_INTER_LINEAR);
    leftImgFrame = leftDummyImage;

    cv::Mat rightDummyImage;
    cv::remap(rightImgFrame, rightDummyImage, this->RightRectMap1, this->RightRectMap2, CV_INTER_LINEAR);
    rightImgFrame = rightDummyImage;

	return 0;

}


void dockingport_Rectifier::getCameraParameters(cv::Mat& Qin, cv::Mat& Rin, cv::Mat& Tin, cv::Mat& R1in, cv::Mat& P1in,
		cv::Mat& R2in, cv::Mat& P2in, cv::Mat& M1in, cv::Mat& D1in, cv::Mat& M2in, cv::Mat& D2in,
		double& Txin, double& Tyin, double& Tzin, double& fin, double& cxin, double& cyin)
{

    Qin = this->Q;
    Rin = this->R;
    Tin = this->T;
    R1in = this->R1;
    P1in = this->P1;
    R2in = this->R2;
    P2in = this->P2;
    M1in = this->M1;
    D1in = this->D1;
    M2in = this->M2;
    D2in = this->D2;
    Txin = this->Tx;
    Tyin = this->Ty;
    Tzin = this->Tz;
    fin = this->f;
    cxin = this->cx;
    cyin = this->cy;

}

void dockingport_Rectifier::getCameraParameters(cv::Mat& Qin)
{
    Qin = this->Q;
}


void dockingport_Rectifier::getCameraParameters(cv::Mat& Rin, cv::Mat& Tin, cv::Mat& M1in, cv::Mat& D1in, cv::Mat& M2in, cv::Mat& D2in) 
{
    Rin = this->R;
    Tin = this->T;
    M1in = this->M1;
    D1in = this->D1;
    M2in = this->M2;
    D2in = this->D2;
}

void dockingport_Rectifier::getCameraParameters(double & cx_left,double &  cy_left,double &  cx_right,double &  cy_right,double &  f_left,double &  f_right,double &  Tx,double &  Ty,double &  Tz) 
{
	cx_left = this->M1.at<double>(0,2);
	cy_left = this->M1.at<double>(1,2);
	cx_right = this->M2.at<double>(0,2);
	cy_right = this->M2.at<double>(1,2);
	f_left = this->M1.at<double>(0,0);
	f_right = this->M2.at<double>(0,0);
	Tx = this->Tx;
	Ty = this->Ty;
	Tz = this->Tz;
}
