/*! 
* 	\file    orfGSP.cpp
* 	\author  Gabriel Urbain <gurbain@mit.edu> - Visitor student at MIT SSL
* 	\date    July 2014
* 	\version 0.1
* 	\brief   Guest Scientist Program for ORF in HaloCore
*
* 	License: The MIT License (MIT)
* 	Copyright (c) 2014, Massachussets Institute of Technology
*/

#include "orfGSP.h"

///////////////////////////////////////////////////////////////////////////////////////////

// timestampedsingleImage::timestampedsingleImage(double _timestamp, cv::Mat& singleImage)
// {
// 	timestamp = _timestamp;
// 	storeimg.create(cv::Size(640,480), CV_8UC3);
// 	singlestoreimg = storeimg(cv::Rect(0,0,640,480));
// 	singleImage.copyTo(singlestoreimg);
// }
// 
// void timestampedsingleImage::writeImg(dockingport_DataStorage& ds) 
// {
// 	ds.timetagStorageFile << ds.numsaved << "," << timestamp << "\n";
// 	sprintf(ds.singleImageStorageName, "%s/SingleImage%d.bmp", ds.imagestoragepath, ds.numsaved);
// 	cv::imwrite(ds.singleImageStorageName, storeimg);
// 	ds.numsaved++;
// }
// 
// void timestampedsingleImage::deallocImage() 
// {
// 	storeimg.deallocate();
// }
// 
// double timestampedsingleImage::getTimestamp() 
// {
// 	return timestamp;
// }

///////////////////////////////////////////////////////////////////////////////////////////

orfGSP::orfGSP(haloGSP *halo)
{ 
	auto_exposure = true;
	integration_time = 100;
	modulation_freq = 21;
	amp_threshold = 20;
	ether_addr = "192.168.1.42";
	calib_filename = "ORF_calib.xml";
	save_dir = "img";
}

void orfGSP::shutdown()
{
	orf.close();
}

void orfGSP::captureAndRectify(cv::Mat& depthImg, cv::Mat& visualImg, cv::Mat& confidencyImg)
{
	TimeStamp ts;
	orf.captureRectified(depthImg, visualImg, confidencyImg, ts);
}

void orfGSP::captureAndSaveRectified()
{
	orf.saveRectified();
}

int orfGSP::init()
{
	int retVal = orf.init(this->auto_exposure, this->integration_time, this->modulation_freq, this->amp_threshold, this->ether_addr, this->save_dir, this->calib_filename);
	
	return retVal;
}

void orfGSP::parseParameterFile(string line, bool printParams)
{
	string searchString;
	string foundString;
	size_t found;

	// Set auto-exposure
	searchString = "ORF_AUTO_EXPOSURE";
	found = line.find(searchString);
	if (found != string::npos) {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString == "true") {
			this->auto_exposure = true;
		}
		if (foundString == "false") {
			this->auto_exposure = false;
		}
		if(printParams) {
			cout << "ORF_AUTO_EXPOSURE " << this->auto_exposure << endl;
		}
	}
	searchString.clear();
	found = string::npos;
	
	// Set amplitude treshold
	searchString = "ORF_AMP_TRESH";
	found = line.find(searchString);
	if (found != string::npos) {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		this->amp_threshold =  atoi(foundString.c_str());
		if(printParams) {
			cout << "ORF_AMP_TRESH " << this->amp_threshold << endl;
		}
	}
	searchString.clear();
	found = string::npos;
	
	// Set modulation frequency
	searchString = "ORF_MOD_FREQ";
	found = line.find(searchString);
	if (found != string::npos) {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		this->modulation_freq =  atoi(foundString.c_str());
		if(printParams) {
			cout << "ORF_MOD_FREQ " << this->modulation_freq << endl;
		}
	}
	searchString.clear();
	found = string::npos;
	
	// Set integration time
	searchString = "ORF_INTEGRATION_TIME";
	found = line.find(searchString);
	if (found != string::npos) {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		this->integration_time =  atoi(foundString.c_str());
		if(printParams) {
			cout << "ORF_INTEGRATION_TIME " << this->integration_time << endl;
		}
	}
	searchString.clear();
	found = string::npos;
	
	// Set IP adress
	searchString = "ORF_IP_ADDR";
	found = line.find(searchString);
	if (found != string::npos) {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		this->ether_addr =  foundString.c_str();
		if(printParams) {
			cout << "ORF_IP_ADDR " << this->ether_addr << endl;
		}
	}
	searchString.clear();
	found = string::npos;
	
	// Set IP adress
	searchString = "ORF_CALIB_FILENAME";
	found = line.find(searchString);
	if (found != string::npos) {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		this->calib_filename =  foundString.c_str();
		if(printParams) {
			cout << "ORF_CALIB_FILENAME " << this->calib_filename << endl;
		}
	}
	searchString.clear();
	found = string::npos;
	
	// Set IP adress
	searchString = "ORF_SAVE_DIR";
	found = line.find(searchString);
	if (found != string::npos) {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		this->save_dir =  foundString.c_str();
		if(printParams) {
			cout << "ORF_SAVE_DIR " << this->save_dir << endl;
		}
	}
	searchString.clear();
	found = string::npos;
} 
