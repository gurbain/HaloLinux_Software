/*! 
* 	\file    thermocamGSP.cpp
* 	\author  Gabriel Urbain <gurbain@mit.edu> - Visitor student at MIT SSL
* 	\date    July 2014
* 	\version 0.1
* 	\brief   Guest Scientist Program for Thermographic camera in HaloCore
*
* 	License: The MIT License (MIT)
* 	Copyright (c) 2014, Massachussets Institute of Technology
*/

#include "thermocamGSP.h"

thermocamGSP::thermocamGSP(haloGSP *halo)
{ 

}

void thermocamGSP::shutdown()
{
	thermo.close();
}

void thermocamGSP::capture(cv::Mat& irImg)
{
	TimeStamp ts;
	thermo.capture(irImg, ts);
}

void thermocamGSP::captureAndSave()
{
	thermo.captureAndSave();
}

int thermocamGSP::init()
{
	int retVal = thermo.init(this->save_dir);
	
	return retVal;
}

void thermocamGSP::parseParameterFile(string line, bool printParams)
{
	string searchString;
	string foundString;
	size_t found;

	// Set IP adress
	searchString = "THERMOCAM_SAVE_DIR";
	found = line.find(searchString);
	if (found != string::npos) {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		this->save_dir =  foundString.c_str();
		if(printParams) {
			cout << "THERMOCAM_SAVE_DIR " << this->save_dir << endl;
		}
	}
	searchString.clear();
	found = string::npos;
} 
