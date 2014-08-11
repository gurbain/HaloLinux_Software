#include "testproject.h"

testproject::testproject () {
	maneuverNumber = 1;
	averageTime = 0;
	time_samples = 0;
	max_depth = 100.0;
	min_depth = 0.01;
	SubSample = false;
	equalize = false;
	pthread_mutex_init(&keymutex, NULL);
}

void testproject::GSsetup(){
	dockingportGSP::init();
	this->videostreaming.createNew_MatVideoBuffer(singleVideoBuffer,"Docking Port View");
	this->videostreaming.setAsDefaultVideoMode(singleVideoBuffer);

	save_frame_number = 0;

	this->initVideoStreaming();
	cout << "past init video" << endl;
	this->captureAndRectifySingleImage(this->singleImage);

}

void testproject::GSrunMain()
{
	this->captureAndRectifySingleImage(this->singleImage);
	this->GSprocessSingleImage(this->singleImage);

}

void testproject::GSprocessSingleImage(cv::Mat& singleImg){

	std::cout << "Processing single image\n";
	cv::Mat filteredRImg[2]=Mat::zeros(singleImg.rows, singleImg.cols, CV_8UC3);// filtered red image
	cv::Mat filteredGImg[2];// filtered Green image
	cv::Mat filteredBImg[2];// filtered Blue image
	cv::Mat filteredSplitImg[2];

	cv::Mat zerosImg[1]=Mat::zeros(singleImg.rows, singleImg.cols, CV_8UC1);;// filtered image

	cv::Mat Rfilt, Gfilt, Bfilt;
	cv::Mat splitImgB[3];
	cv::Mat splitImgG[3];
	cv::Mat splitImgR[3];
	cv::Mat greyImg = singleImg;

	Bfilt.create(camera.getImageHeight(), camera.getImageWidth(), CV_8UC3);
	Gfilt.create(camera.getImageHeight(), camera.getImageWidth(), CV_8UC3);
	Rfilt.create(camera.getImageHeight(), camera.getImageWidth(), CV_8UC3);

	split(singleImg, splitImgB);
	split(singleImg, splitImgG);
	split(singleImg, splitImgR);


//	splitImgB[1] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*0;
//	splitImgB[2] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*0;
//	splitImgG[0] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*0;
//	splitImgG[2] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*0;
//	splitImgR[0] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*0;
//	splitImgR[1] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*0;

	splitImgB[1] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*255;
	splitImgB[2] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*255;
	splitImgG[0] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*255;
	splitImgG[2] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*255;
	splitImgR[0] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*255;
	splitImgR[1] = Mat::ones(singleImg.rows, singleImg.cols, CV_8UC1)*255;

//	zerosImg[0]=Mat::zeros(singleImg.rows, singleImg.cols, CV_8UC1);
//	zerosImg[1]=Mat::zeros(singleImg.rows, singleImg.cols, CV_8UC1);


//	filteredSplitImg.create(cameras.getImageHeight(), cameras.getImageWidth(), CV_8UC1);

//	splitImg[0]=Mat::zeros(singleImg.rows, singleImg.cols, CV_8UC1);

//	adaptiveThreshold(splitImgB[0], splitImgB[0], 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 11,17);
//	adaptiveThreshold(splitImgG[1], splitImgG[1], 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 11,17);
//	adaptiveThreshold(splitImgR[2], splitImgR[2], 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 11,17);

	adaptiveThreshold(splitImgB[0], splitImgB[0], 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15,30);
	adaptiveThreshold(splitImgG[1], splitImgG[1], 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 15,35);
	adaptiveThreshold(splitImgR[2], splitImgR[2], 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17,40);

//	adaptiveThreshold(splitImgB[0], splitImgB[0], 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 11,17);
//	adaptiveThreshold(splitImgB[1], splitImgB[1], 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 11,17);
//	adaptiveThreshold(splitImgB[2], splitImgB[2], 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 11,17);


	merge(splitImgB,3,Bfilt);
	merge(splitImgG,3,Gfilt);
	merge(splitImgR,3,Rfilt);


//	merge(filteredRImg,3,singleImg);
//std::cout<<filteredRImg[1];

//	singleImg = Mat::zeros(singleImg.rows, singleImg.cols, CV_8UC3);
	cv::imwrite("/home/goggles/Desktop/test_Bfilt.bmp", Bfilt);
	cv::imwrite("/home/goggles/Desktop/test_Gfilt.bmp", Gfilt);
	cv::imwrite("/home/goggles/Desktop/test_Rfilt.bmp", Rfilt);

	//Do it in grey
	cv::cvtColor(singleImg, greyImg, CV_BGR2GRAY);
	adaptiveThreshold(greyImg, greyImg, 0xFF, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 17,13);
	cv::imwrite("/home/goggles/Desktop/test_grey.bmp", greyImg);

	if (this->datastorage.autoImageStorage) {
			stringstream filename;
			filename << this->datastorage.getGSdatastoragePath() << "DockingPortImg" << save_frame_number << ".bmp";
			cv::imwrite(filename.str(), singleImg);
			filename.clear();

			save_frame_number++;
	}

	if (videostreaming.videoStreamingOn) {
		videostreaming.update_MatVideoBuffer(singleVideoBuffer, singleImg);
	}


}

void testproject::GSbackgroundTask() {

}

void testproject::GSparseParameterFile(string line) {
	  string searchString;
	  string foundString;
	  size_t found;
	  
	  dockingportGSP::parseParameterFile(line);

	  searchString = "SUBSAMPLE";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString == "true") {
			this->SubSample = true;
		}
		cout << "SUBSAMPLE:  " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  searchString = "EQUALIZE";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString == "true") {
			this->equalize = true;
		}
		cout << "EQUALIZE:  " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  searchString = "MIN_DEPTH";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  min_depth =  atof(foundString.c_str());
		  cout << "MIN_DEPTH: " << min_depth << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  searchString = "MAX_DEPTH";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  max_depth =  atof(foundString.c_str());
		  cout << "MAX_DEPTH: " << max_depth << endl;
	  }
	  searchString.clear();
	  found = string::npos;
}

void testproject::GScleanup() {
	
	dockingportGSP::shutdown();

}

void testproject::GSparseCommandlineArgs(int argc, char *argv[]) {

// Read in path from environment variable
	if (argc>3) {

		sprintf(this->calibParamSetName, "%s", argv[3]);

	}
	else {

		cout << endl << endl << "Not enough command line arguments" << endl << "Please specify path to Camera Calibration Parameters!" << endl;

		cout << "Usage: <name of camera calibration set>" << endl << endl;

		exit(1);

	}


}


