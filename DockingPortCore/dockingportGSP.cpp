#include "dockingportGSP.h"

void dockingportGSP::parseParameterFile(string line, bool printParams) 
{
    string searchString;
	string foundString;
	size_t found;

    //use UDP PIC
    searchString = "USE_UDP_PIC";
    found = line.find(searchString);
    if (found != string::npos)
    {
  	  foundString = line.substr( found+searchString.size()+1, string::npos );
	  if (foundString == "true") 
	  {
		  this->useUDP_PIC = true;
	  }
	  if (foundString == "false") 
	  {
		  this->useUDP_PIC = false;
	  }
	  if(printParams)
	  {
  		  cout << "USE_UDP_PIC " << foundString << endl;
	  }
    }
    searchString.clear();
    found = string::npos;

    //frameRate
    searchString = "FRAME_RATE_UDP";
    found = line.find(searchString);
    if (found != string::npos)
    {
	    foundString = line.substr( found+searchString.size()+1, string::npos );
	    this->camera.setFrameRate( atof(foundString.c_str()) );
	    if(printParams)
	    {
  		    cout << "FRAME_RATE_UDP " << foundString << endl;
	    }
    }
    searchString.clear();
    found = string::npos;

    // exposureTime
    searchString = "EXPOSURE_TIME_UDP";
    found = line.find(searchString);
    if (found != string::npos)
    {
	    foundString = line.substr( found+searchString.size()+1, string::npos );
	    this->camera.setExposureTime( atoi(foundString.c_str()) );
	    if(printParams)
	    {
  		    cout << "EXPOSURE_TIME_UDP " << foundString << endl;
	    }
    }
    searchString.clear();
    found = string::npos;

    // HW gain
    searchString = "HW_GAIN_UDP";
    found = line.find(searchString);
    if (found != string::npos)
    {
	    foundString = line.substr( found+searchString.size()+1, string::npos );
	    this->camera.setHWGain( atoi(foundString.c_str()) );
	    if(printParams)
	    {
  		    cout << "HW_GAIN_UDP " << foundString << endl;
	    }
    }
    searchString.clear();
    found = string::npos;

    // use images from file
    searchString = "UDP_INPUT_IMG_DIR";
    found = line.find(searchString);
    if (found != string::npos)
    {
	  foundString = line.substr( found+searchString.size()+1, string::npos );
	  if (foundString != "false") 
	  {
	  	  this->camInputImgDir = foundString;
	  	  this->camera.useCamera = false;
	  	  if(printParams)
	      {
  		      cout << "UDP_INPUT_IMG_DIR " << foundString << endl;
	      }
	  }
    }
    searchString.clear();
    found = string::npos;

    // start image
    searchString = "UDP_INPUT_START_IMG";
    found = line.find(searchString);
    if (found != string::npos)
    {
	    foundString = line.substr( found+searchString.size()+1, string::npos );
	    this->camInputStartImg =  atoi(foundString.c_str());
	    if(printParams)
	    {
  		    cout << "UDP_INPUT_START_IMG " << this->camInputStartImg << endl;
	    }
    }
    searchString.clear();
    found = string::npos;

    // final image
    searchString = "UDP_INPUT_FINAL_IMG";
    found = line.find(searchString);
    if (found != string::npos)
    {
	    foundString = line.substr( found+searchString.size()+1, string::npos );
	    this->camInputFinalImg = atoi(foundString.c_str());
	    if(printParams)
	    {
  		    cout << "UDP_INPUT_FINAL_IMG " << this->camInputFinalImg << endl;
	    }
    }
    searchString.clear();
    found = string::npos;

    // automatic image storage
    searchString = "AUTOMATIC_IMAGE_STORAGE_UDP";
    found = line.find(searchString);
    if (found != string::npos)
    {
	    foundString = line.substr( found+searchString.size()+1, string::npos );
	    if (foundString == "true")
		{
		    this->datastorage.autoImageStorage = true;
		}
	    if (foundString == "false")
		{
		    this->datastorage.autoImageStorage = false;
		}
	    if(printParams)
	    {
  		    cout << "AUTOMATIC_IMAGE_STORAGE_UDP " << foundString << endl;
	    }
     }
    searchString.clear();
    found = string::npos;

    // unrectified image storage
    searchString = "UNRECTIFIED_IMAGE_STORAGE_UDP";
    found = line.find(searchString);
    if (found != string::npos)
    {
	    foundString = line.substr( found+searchString.size()+1, string::npos );
	    if (foundString == "true")
		{
		  this->datastorage.unrectifiedImageStorage = true;
		}
	    if (foundString == "false")
		{
		  this->datastorage.unrectifiedImageStorage = false;
		}
	    if(printParams)
	    {
  		  cout << "UNRECTIFIED_IMAGE_STORAGE_UDP " << foundString << endl;
	    }
    }
    searchString.clear();
    found = string::npos;

    // useOneCamera
    searchString = "USE_UDP_CAM";
    found = line.find(searchString);
    if (found != string::npos)
    {
	  foundString = line.substr( found+searchString.size()+1, string::npos );
	  if (foundString == "true") 
	  {
	  	  this->camera.useCamera = true;
	  }
	  if (foundString == "false") 
	  {
	  	  this->camera.useCamera = false;
	  }
	  if(printParams)
	  {
  		  cout << "USE_UDP_CAM " << foundString << endl;
	  }
    }
    searchString.clear();
    found = string::npos;
} 

void dockingportGSP::parseUDPSerialFile(){
	string line;
	size_t found;
	size_t nextFound;
	
	//Open the UDP_SERIAL_FILE
	ifstream UDPSerialFile(UDP_SERIAL_FILE);
	if (UDPSerialFile.is_open()) 
	{
		cout << "---------------------------" << endl;
		cout << endl << "------ UDPSerialFile ------" << endl;
		
		for (int count = 0; count < 100; count++) 
		{
			getline(UDPSerialFile,line);
			if (UDPSerialFile.eof()) 
			{
				camera.OPTICS_ID[count] = -1;
				goto close; //closes UDP_SERIAL_FILE
			}
			//store UDP_ID
			found = line.find(":");
			strcpy(UDP_ID[count],line.substr(0,found).c_str());
			if(strcmp(UDP_storage_ID,UDP_ID[count])==0)
			{
				foundUDPID = true;
			}
			
			else
			{
				foundUDPID = false; //ensures that UDP doesn't point to wrong camera and FTDI IDs when looped thru
			}

			//store CAM_SERIAL
			nextFound = line.find(":",found+1); //stores upto next occurrence of ":"
			found = line.find(":",found); //starts at first ":"
			strcpy(camera.SINGLE_CAM_SERIAL[count],line.substr(found+1,nextFound-2).c_str()); //makes sure array doesn't store ":" in entries

			//store OPTICS_ID
			nextFound = line.find(":",found); //stores upto next occurrence of ":"
			found = line.find(":",found+1); //starts at next occurrence of ":"
			camera.OPTICS_ID[count] = atoi(line.substr(found+1,nextFound+2).c_str()); //makes sure array doesn't store ":" in entries
			if(foundUDPID)
			{
				camera.sethCam(camera.OPTICS_ID[count]);
				camera_ID = camera.OPTICS_ID[count]; //used to set correct datastorage folder name
			}
				
			//store FTDI_ID
			found = line.find(":",found+1); //starts at next occurrence of ":"
			strcpy(FTDI_serID[count],line.substr(found+1,string::npos).c_str()); //reads until end of line
			if(foundUDPID)
			{
				FTDI_ID = FTDI_serID[count]; //used to store FTDI_ID
			}

			printf("UDP S/N: %s CAMERA S/N & ID: %s %d FTDI S/N: %s\n",UDP_ID[count],camera.SINGLE_CAM_SERIAL[count],camera.OPTICS_ID[count],FTDI_serID[count]);
		}
	}

	else 
	{
		cout << "Unable to open: " << UDP_SERIAL_FILE << endl;
	}

	close:
	{
		UDPSerialFile.close();
	}
}

// Copied to haloGSP (called dataStorage_thread())
void dockingportGSP::imageStorage_thread() 
{

	while(!stopImageStorageThread)
	{
		if (!storageImg_deque.empty()) 
		{
			struct timespec timeA, timeB;
			clock_gettime(CLOCK_REALTIME, &timeA);
			int deque_size = storageImg_deque.size();

			pthread_mutex_lock(&storageDeque_mutex);
			timestampedsingleImage& timg = storageImg_deque.back();
			pthread_mutex_unlock(&storageDeque_mutex);

			timg.writeImg(this->datastorage);

			pthread_mutex_lock(&storageDeque_mutex);
			storageImg_deque.pop_back();
			pthread_mutex_unlock(&storageDeque_mutex);
			timg.deallocImage();

			clock_gettime(CLOCK_REALTIME, &timeB);
			double dt = timeDiff(&timeA, &timeB);
			if (deque_size > 1) 
			{
				std::cout << "DP Datastorage Deque Size: " << deque_size << ". Timestamp: " << timg.getTimestamp() << "Write time: " << dt << std::endl;
			}

		} 
		else 
		{
			storageImg_deque.clear();
			usleep(10000);
		}
	}
	pthread_exit(0);
}

void dockingportGSP::init()
{
/////////////////// Set process priorities

	int prior;
	int dummy1;
	///  set process priority of ueye usb daemon to highest possible value
	char reniceCmd[100];
	string processIDofUeyeDeamonStr;
	int processIDofUeyeDeamon;
	char pidOfCmd[100];
	sprintf(pidOfCmd, "pidof ueyeusbd");

	processIDofUeyeDeamonStr = this->execAndReturnSystemOutput(pidOfCmd);
	processIDofUeyeDeamon = atoi(processIDofUeyeDeamonStr.c_str());
	sprintf(reniceCmd, "sudo renice -20 %d", processIDofUeyeDeamon);
	dummy1 = system(reniceCmd);
	prior = getpriority(PRIO_PROCESS, processIDofUeyeDeamon);
	printf("\n ueyeusbd process priority set to: %d\n",prior);

	///  set process priority of TestProject process to a value slightly lower than ueye usb daemon
	setpriority(PRIO_PROCESS,0,-15);
	usleep(10000);
	prior = getpriority(PRIO_PROCESS,0);
	printf(" TestProject process priority set to: %d\n",prior);

	this->useBackgroundTask = false;

	prevImageTimeStamp = 0;

/////////////////// Path Initializations

	sprintf(this->calibParamDir, "/opt/GogglesOptics/Calib_Params/%s", this->calibParamSetName); //set the parameter directory to be what we enter in command line

///////////////////

/////////////////// Camera Initialization section
	GSPretVal = camera.initOneCamera(); // Errors handled within functions
	if (GSPretVal != 0)
	{
		printf("Camera could not be initialized!\n");
		return exit(1);
	}
	// Start camera

	GSPretVal = camera.startOneCamera(); // Errors handled within functions
	if (GSPretVal != 0)
	{
		printf("Camera could not be started!\n");
		return exit(1);
	}


////////////////// UDP PIC INITIALIZATION
	if (useUDP_PIC)
		this->connectToPIC();

///////////////////

/////////////////// Image Rectification Initialization

	GSPretVal = rectifier.calcRectificationMaps(camera.getImageWidth(), camera.getImageHeight(), this->calibParamDir);
	if (GSPretVal != 0)
	{
		cout << "Rectification maps could not be processed!" << endl;
	}

///////////////////

/////////////////// Data Storage Initialization
	datastorage.initDataStorage(this->dockportName, this->camera_ID, this->runPath, this->camera.getImageWidth(), this->camera.getImageHeight());
	if (datastorage.autoImageStorage) 
	{

		// create image-datastorage thread
		GSPretVal = pthread_create(&ImageStorageThread, NULL, this->imageStorage_thread_Helper, this);
		if (GSPretVal != 0)
		{
			printf("pthread_create ImageStorageThread failed\n");
			return exit(1);
		}

	}
///////////////////

/////////////////// Initialization of OpenCV Mats with correct Size (depending on the parameter/boolean camera.reduceImageSizeTo320x240)

	singleImage.create(camera.getImageHeight(), camera.getImageWidth(), CV_8UC3);

///////////////////

}

void dockingportGSP::captureAndRectifySingleImage(cv::Mat& singleImg) //main function for image capture
{
	if (camera.useCamera)
	{
		GSPretVal = camera.captureOneImage(singleImg);
		if (GSPretVal != 0)
		{
			printf("Capture One Image Failed\n");
		}

		this->updateTime();

		this->prevImageTimeStamp = this->imageTimeStamp;
		this->imageTimeStamp = this->testtime;
		double deltaTime = imageTimeStamp - prevImageTimeStamp;

		if (deltaTime > 2*(1000.0 / this->camera.getFrameRate())) 
		{
	         ///std::cout << "Lrg DeltaTime: " << deltaTime << "at: " << imageTimeStamp << std::endl;
		}
	} 

	else 
	{
		stringstream currFileName;
		cv::Mat currCombinedImg;
		double imgTimestep;

		imgTimestep = 1.0E6 / this->camera.getFrameRate();
		this->currentTime;
		usleep(imgTimestep);

		if(this->camInputImgCounter < this->camInputStartImg) 
		{
			this->camInputImgCounter = this->camInputStartImg;
		}

		currFileName.str("");
		currFileName << this->camInputImgDir << "/SingleImage" << this->camInputImgCounter << ".bmp";

		//end if less than 0 or if greater than specified number
		if(this->camInputFinalImg > 0 && this->camInputImgCounter > this->camInputFinalImg) 
		{
			std::cout << "Last Image read: " << this->camInputImgCounter << std::endl;
			this->shutdownCriterion = true;
		}

		//std::cout << "Reading Image: " << currFileName.str() << std::endl;

		singleImg = cv::imread(currFileName.str());

		if (singleImg.empty()) 
		{
			std::cout << "Empty Image: " << currFileName.str() << std::endl;

			if (this->camInputFinalImg == -1) 
			{
				this->shutdownCriterion = true;
			} 
			else if (this->camInputFinalImg == -10) 
			{
				this->camInputImgCounter = this->camInputStartImg;
				currFileName.str("");
				currFileName << this->camInputImgDir << "/SingleImage" << this->camInputImgCounter << ".bmp";

				std::cout << "Re-Reading Image: " << currFileName.str() << std::endl;
				singleImg = cv::imread(currFileName.str());

				if (singleImg.empty()) 
				{
					std::cout << "Two Empty Images, shutting down: " << currFileName.str();
					this->shutdownCriterion = true;
					return;
				}
			}

		}

	    //cvtColor( singleImg, singleImg, CV_BGR2GRAY );

		currFileName.str("");
		this->camInputImgCounter++;

		this->updateTime();

		this->prevImageTimeStamp = this->imageTimeStamp;
		this->imageTimeStamp = this->testtime;
		double deltaTime = imageTimeStamp - prevImageTimeStamp;

		if (deltaTime > 2*(1000.0 / this->camera.getFrameRate())) 
		{
			//std::cout << "Lrg DeltaTime: " << deltaTime << "at: " << imageTimeStamp << std::endl;
		}

	}

	struct timespec timeA, timeB;
	clock_gettime(CLOCK_REALTIME, &timeA);

	timestampedsingleImage tImage(this->imageTimeStamp, singleImg);

	pthread_mutex_lock(&storageDeque_mutex);
	storageImg_deque.push_front(tImage);
	pthread_mutex_unlock(&storageDeque_mutex);

	clock_gettime(CLOCK_REALTIME, &timeB);
	double save_time_diff = timeDiff(&timeA, &timeB);
	//std::cout << "Time to add to deque: " << save_time_diff << std::endl;

}

void dockingportGSP::connectToPIC()
{
	string originalWD;
	originalWD = this->ftdi.getCWD(); //saves current working directory to be reset later

	// Saves connected FTDI Serial IDs to ftdi.serialNumbers and saves 'ttyUSBX' to ftdi.serialPaths in order
	ftdi.run();

	serPath = ftdi.checkSerial(FTDI_ID); // Checks if requested ID matches any of connected IDs, saves path to appropriate connected device to serPath
	if(serPath=="")
	{
		printf("FTDI ID not found in connected ports!\n");
		return exit(1);
	}
	string openDir = "/dev/";
	openDir += serPath; //updates openDir, now should look like "/dev/ttyUSB(#)"

	//open comm port with right path
	GSPretVal = PIC.Open(openDir.c_str(), 115200); //using baudrate of 115200
	switch(GSPretVal)
	{
		case 1: cout << "Comm port opened successfully!" << endl; break;
		case -1: cout << "Device not found" << endl; break;
		case -2: cout << "Error while opening the device" << endl; break;
		case -3: cout << "Error while getting port parameters" << endl; break;
		case -4: cout << "Speed (Bauds) not recognized" << endl; break;
		case -5: cout << "Error while writing port parameters" << endl; break;
		case -6: cout << "Error while writing timeout parameters" << endl; break;
		default: cout << "unknown error" << endl;
	}

	this->ftdi.changeWD(originalWD); //reset working directory to original working directory

}

void dockingportGSP::motorForward() 
{

	GSPretVal = PIC.WriteChar(0x43);
	switch (GSPretVal) 
	{
	case 1 : cout << "Written successfully." << endl; break;
	case -1: cout << "Error writing to UDP PIC." << endl; break;
	default: cout << "Unknown error occurred." << endl; break;
	}
}

void dockingportGSP::motorReverse() 
{

	GSPretVal = PIC.WriteChar(0x41);
	switch (GSPretVal) 
	{
	case 1 : cout << "Written successfully." << endl; break;
	case -1: cout << "Error writing to UDP PIC." << endl; break;
	default: cout << "Unknown error occurred." << endl; break;
	}
}

void dockingportGSP::motorOff() {

	GSPretVal = PIC.WriteChar(0x40);
	switch(GSPretVal) 
	{
	case 1 : cout << "Written successfully." << endl; break;
	case -1: cout << "Error writing to UDP PIC." << endl; break;
	default: cout << "Unknown error occurred." << endl; break;
	}
}

unsigned char dockingportGSP::readPIC() 
{

	GSPretVal = PIC.WriteChar(0x73); //first hex digit must be 7, second one doesn't matter
	switch (GSPretVal) 
	{
	case 1 : cout << "Written successfully." << endl; break;
	case -1: cout << "Error writing to UDP PIC." << endl; break;
	default: cout << "Unknown error occured." << endl; break;
	}
	GSPretVal = PIC.ReadChar(Buffer_packet, 5000);
	switch (GSPretVal) {
	case 1 : cout << "Written successfully." << endl; break;
	case -1: cout << "Error writing to UDP PIC." << endl; break;
	case -2: cout << "Error while reading the byte." << endl; break;
	case 0: cout << "Timeout reached while reading byte." << endl; break;
	default: cout << "Unknown error occured." << endl; break;
	}
	pic_byte = (unsigned char) *Buffer_packet;
	return pic_byte;
}

void dockingportGSP::shutdown() 
{

	if(camera.useCamera)
	{
		try
		{
			GSPretVal = camera.stopOneCamera();
			if (GSPretVal != 0)
				throw "Camera could not be stopped!";
		}
		catch (char * str) 
		{
			cout << "Exception raised: " << str << '\n';
		}

		try
		{
			GSPretVal = camera.closeOneCamera();
			if (GSPretVal != 0)
				throw "Camera could not be closed!";
		}
		catch (char * str) 
		{
			cout << "Exception raised: " << str << '\n';
		}

		sleep(2);

		// shut down data storage
		if(datastorage.autoImageStorage) 
		{
			int storimgshutcnt = 0;
			int timeout_delay = 500000;
			//wait for storage image deque to clear
			while(storageImg_deque.size() > 0) 
			{
				if (storimgshutcnt++ > (20*1000000 / timeout_delay)) 
				{
					std::cout << "Timeout waiting for img deque\n";
					break;
				}
				std::cout << "Shutdown delayed - storageImg_deque.size() = " << storageImg_deque.size() << std::endl;
				std::cout << "Storage Img Shutdown Counter: " << storimgshutcnt << std::endl;
				usleep(timeout_delay); //sleep 0.1 seconds
			}

			stopImageStorageThread = true;
			usleep(timeout_delay);

			datastorage.shutdownDataStorage();
			sleep(1);
		}
	}

	if(useUDP_PIC)
	{
		//close serial port
		PIC.Close();
		printf("Closed comm port!\n");
	}
}
