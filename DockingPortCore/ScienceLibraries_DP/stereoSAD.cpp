#include "stereoSAD.h"


void StereoSAD::computeDisparity(cv::Mat& cvFrame1, cv::Mat& cvFrame2, cv::Mat& disparityMap) {


    //set bm parameters
    bm.state->roi1 = roi1;
    bm.state->roi2 = roi2;
    //bm.state->preFilterType = CV_STEREO_BM_NORMALIZED_RESPONSE;
    bm.state->preFilterCap = 31;
    bm.state->SADWindowSize = SADWindowSize > 0 ? SADWindowSize : 9;
    bm.state->minDisparity = minDisparity;
    bm.state->numberOfDisparities = numberOfDisparities;
    bm.state->textureThreshold = 10;
    bm.state->uniquenessRatio = 15;
    bm.state->speckleWindowSize = 100;
    bm.state->speckleRange = 32;
    bm.state->disp12MaxDiff = 1;	//lr check if >0

    //perform block matching
	bm(cvFrame1, cvFrame2, disp);

	//convert and output
    disp.convertTo(disparityMap, CV_8U, 255/(numberOfDisparities*16.));

}

void StereoSAD::computeDisparity(cv::Mat& cvFrame1, cv::Mat& cvFrame2, cv::Mat& disparityMap, cv::Mat& disp) {


    //set bm parameters
    bm.state->roi1 = roi1;
    bm.state->roi2 = roi2;
    //bm.state->preFilterType = CV_STEREO_BM_NORMALIZED_RESPONSE;
    bm.state->preFilterCap = 31;
    bm.state->SADWindowSize = SADWindowSize > 0 ? SADWindowSize : 9;
    bm.state->minDisparity = minDisparity;
    bm.state->numberOfDisparities = numberOfDisparities;
    bm.state->textureThreshold = 10;
    bm.state->uniquenessRatio = 15;
    bm.state->speckleWindowSize = 100;
    bm.state->speckleRange = 32;
    bm.state->disp12MaxDiff = 1;	//lr check if >0

    //perform block matching
	bm(cvFrame1, cvFrame2, disp);

	//convert and output
    disp.convertTo(disparityMap, CV_8U, 255/(numberOfDisparities*16.));

}

