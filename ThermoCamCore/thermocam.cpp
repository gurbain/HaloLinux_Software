/*! 
* 	\file    thermocam.cpp
* 	\author  Gabriel Urbain <gurbain@mit.edu> - Visitor student at MIT SSL
* 	\date    August 2014
* 	\version 0.1
* 	\brief   Sources for ethernet thermographic camera
*
* 	License: The MIT License (MIT)
* 	Copyright (c) 2014, Massachussets Institute of Technology
*/

#include "thermocam.h"

using namespace std;
using namespace cv;


Thermocam::Thermocam()
{
	this->lDeviceInfo = NULL;
	this->lCountVal = 0;
	this->lFrameRateVal = 0.0;
	this->lBandwidthVal = 0.0;
	this->lWidth = 0;
	this->lHeight = 0;
	this->imgNum = 0;
	this->tslast = 0;
	this->timestamps = "timestampThermo.txt";
}

Thermocam::~Thermocam()
{}

int Thermocam::listCam()
{
	this->lSystem.SetDetectionTimeout( 100 );
	this->lResult = this->lSystem.Find();
	if( !lResult.IsOK() ) //search for ThermoCam
	{
		INFO << "PvSystem::Find Error: " << lResult.GetCodeString().GetAscii();
		return -1;
	}
	this->lInterfaceCount = this->lSystem.GetInterfaceCount();

	for(PvUInt32 x = 0; x < this->lInterfaceCount; x++)
	{
		PvInterface * lInterface = lSystem.GetInterface(x);
		INFO << "Ethernet Interface " << endl;
		INFO << "IP Address: " << lInterface->GetIPAddress().GetAscii() << endl;
		INFO << "Subnet Mask: " << lInterface->GetSubnetMask().GetAscii() << endl << endl;
		PvUInt32 lDeviceCount = lInterface->GetDeviceCount();
		for(PvUInt32 y = 0; y < lDeviceCount ; y++)
		{
			this->lDeviceInfo = lInterface->GetDeviceInfo(y);
			INFO << "Thermocam with IP Address: " << this->lDeviceInfo->GetIPAddress().GetAscii() << " found!"<<endl;
		}
	}

	return 0;
}

int Thermocam::connect()
{
	if (this->lDeviceInfo != NULL) {
		INFO << "Connecting to thermocam: " << this->lDeviceInfo->GetIPAddress().GetAscii() << endl;
		if ( !this->lDevice.Connect( this->lDeviceInfo ).IsOK() ) {
		ERROR << "Unable to connect to " << this->lDeviceInfo->GetIPAddress().GetAscii() << endl;
		return -2;
		}
		INFO << "Successfully connected to thermocam: " << this->lDeviceInfo->GetIPAddress().GetAscii() << endl;
	} else {
		ERROR << "No thermocam found" << endl;
		return -1;
	}
	return 0;
}

void Thermocam::startPipeline()	
{
	// Get device parameters need to control streaming
	this->lDeviceParams = this->lDevice.GetGenParameters();
	
	// Negotiate streaming packet size
	this->lDevice.NegotiatePacketSize();
	
	// Open stream - have the PvDevice do it for us
	INFO << "Opening stream to thermocam" << endl;
	this->lStream.Open( lDeviceInfo->GetIPAddress() );
	DEBUG<<"a"<<endl;

	// Create the PvPipeline object
	this->lPipeline = new PvPipeline( &lStream );

	// Reading payload size from device
	this->lDeviceParams->GetIntegerValue( "PayloadSize", this->lSize );

	// Set the Buffer size and the Buffer count
	this->lPipeline->SetBufferSize( static_cast<PvUInt32>( this->lSize ) );
	this->lPipeline->SetBufferCount( 16 ); 

	// Have to set the Device IP destination to the Stream
	this->lDevice.SetStreamDestination( this->lStream.GetLocalIPAddress(), this->lStream.GetLocalPort() );

	// Start pipeline
	INFO << "Starting thermocam pipeline" << endl;
	this->lPipeline->Start();

	// Get stream parameters/stats
	this->lStreamParams = this->lStream.GetParameters();

	// TLParamsLocked is optional but when present, it MUST be set to 1
	this->lDeviceParams->SetIntegerValue( "TLParamsLocked", 1 );

	INFO << "Resetting thermocam timestamp counter..." << endl;
	this->lDeviceParams->ExecuteCommand( "GevTimestampControlReset" );

	// Start acquisition
	INFO << "Sending StartAcquisition command to thermocam" << endl;
	this->lDeviceParams->ExecuteCommand( "AcquisitionStart" );
}

void Thermocam::close()
{
	INFO << "Sending AcquisitionStop command to thermocam" << endl;

	// Stop the stream
	this->lStop->Execute();

	// If present reset TLParamsLocked to 0 after streaming stop
	this->lDeviceParams->SetIntegerValue( "TLParamsLocked", 0 );

	// Close the pipeline
	INFO << "Stop thermocam pipeline" << endl;
	this->lPipeline->Stop();

	// Close the stream
	INFO << "Closing thermocam stream" << endl;
	this->lStream.Close(); 
	
	// Close the device
	INFO << "Disconnecting thermocam" << endl;
	this->lDevice.Disconnect();
}

int Thermocam::init(string img_directory)
{	
	// Get connected devices list
	int retVal = this->listCam();
	if(retVal!=0)
		return -1;

	// Connect to the devices
	retVal = this->connect();
	if(retVal!=0)
		return -1;

	// Start the pipeline
	startPipeline();

	// Create a new timestamp file and dir
	time_t rawtime;
	struct tm * timeinfo;
	char buffer1[100];
	stringstream buffer2;
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer1,100,"%d-%m-%Y-%I-%M", timeinfo);
	buffer2<<img_directory<<"/"<<buffer1;
	string dir = buffer2.str();
	this->img_directory = dir;
	
	struct stat st;
	if(stat(this->img_directory.c_str(),&st) != 0) {
		INFO<<"Creation of the folder "<<this->img_directory<<endl;
		mkdir(img_directory.c_str(), 0777);
		mkdir(this->img_directory.c_str(), 0777);
	}
	stringstream ss;
	ss<<this->img_directory<<"/timestampThermocam.txt";
	tsfile.open(ss.str().c_str());
	ss.str("");
	if (tsfile.is_open())
		tsfile<<endl<<endl<<"######################### NEW SESSION #######################"<<endl<<endl;

	return 0;
}

int Thermocam::capture(Mat& img, TimeStamp& ts)
{
	// Start timestamp
	ts.start();

	// Retrieve next buffer		
        PvBuffer *lBuffer = NULL;
        PvResult  lOperationResult;
        lResult = lPipeline->RetrieveNextBuffer( &lBuffer, 5000, &lOperationResult );
        

	if ( lResult.IsOK() ) {
		if ( lOperationResult.IsOK() ) {

			lStreamParams->GetIntegerValue( "ImagesCount", lCountVal );
			lStreamParams->GetFloatValue( "AcquisitionRateAverage", lFrameRateVal );
			lStreamParams->GetFloatValue( "BandwidthAverage", lBandwidthVal );

			PvUInt32 lWidth = 0, lHeight = 0;
			if ( lBuffer->GetPayloadType() == PvPayloadTypeImage ) {
				PvImage *lImage = lBuffer->GetImage();
				lWidth = lBuffer->GetImage()->GetWidth();
				lHeight = lBuffer->GetImage()->GetHeight();
				Mat rawlImage(Size(lWidth, lHeight), CV_8U, lBuffer->GetDataPointer());
				img = rawlImage;
			}
		}
		lPipeline->ReleaseBuffer( lBuffer );	
	} else {
		ERROR << "Thermocam acquisition timeout"<<endl;
		return -1;
	}

	// Stop timestamp
	ts.stop();

	return 0;
}

int Thermocam::captureAndSave()
{
	// Create variables to save
	TimeStamp ts;
	Mat irImg;
	
	// Capture images
	this->capture(irImg, ts);
	
	//Save jpg images
	stringstream ss;
	string name = "thermo";
	string type = ".png";
	ss<<this->img_directory<<"/"<<name<<imgNum<<type;
	string filename = ss.str();
	ss.str("");
	
	try {
		imwrite(filename, irImg);
	} catch (int ex) {
		ERROR<<"Exception converting image to png format: "<<ex<<endl;
		return -1;
	}
	
	// Save time stamp
	if (!tsfile.is_open()) {
		tsfile.open(timestamps.c_str());
		if (!tsfile.is_open()) {
			ERROR<<"Impossible to open the file"<<endl;
			return -1;
		}
	}
	if (imgNum==0)
		INFO<<"Saving image files into folder "<<this->img_directory<<endl;
	tsfile<<"IMAGENUM\t"<<imgNum<<"\tPROCTIME\t"<<ts.getProcTime()<<"\tMEANTIME\t"<<ts.getMeanTime()<<"\tDIFF\t"<<ts.getMeanTime()-tslast<<endl;

	imgNum++;
	tslast = ts.getMeanTime();
	
	return 0;


}
