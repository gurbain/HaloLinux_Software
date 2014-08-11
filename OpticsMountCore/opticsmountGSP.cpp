#include "opticsmountGSP.h"

void opticsmountGSP::parseParameterFile(string line, bool printParams) {

      string searchString;
      string foundString;
      size_t found;

	  // frameRate
	  searchString = "FRAME_RATE_OM";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->cameras.setFrameRate( atof(foundString.c_str()) );
		  if(printParams)
	      {
  		      cout << "FRAME_RATE_OM " << foundString << endl;
	      }
	  }
	  searchString.clear();
	  found = string::npos;

	  // exposureTime
	  searchString = "EXPOSURE_TIME_OM";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->cameras.setExposureTime( atoi(foundString.c_str()) );
		  if(printParams)
	      {
  		      cout << "EXPOSURE_TIME_OM " << foundString << endl;
	      }
	  }
	  searchString.clear();
	  found = string::npos;

	  // HW gain
	  searchString = "HW_GAIN_OM";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->cameras.setHWGain( atoi(foundString.c_str()) );
		  if(printParams)
	      {
  		      cout << "HW_GAIN_OM " << foundString << endl;
	      }
	  }
	  searchString.clear();
	  found = string::npos;

	  //useCameras
	  searchString = "USE_OM_CAMS";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString == "true")
		  	this->cameras.useCameras = true;
		if (foundString == "false")
		  	this->cameras.useCameras = false;
		if(printParams)
	    {
  		    cout << "USE_OM_CAMS " << foundString << endl;
	    }
	  }
	  searchString.clear();
	  found = string::npos;

	  // useSynchCams
	  searchString = "USE_SYNCH_CAMS";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  if (foundString == "true")
			  this->cameras.useSynchCams = true;
		  if (foundString == "false")
			  this->cameras.useSynchCams = false;
		  if(printParams)
	      {
  		      cout << "USE_SYNCH_CAMS " << foundString << endl;
	      }
	  }
	  searchString.clear();
	  found = string::npos;

	  // use images from file
	  searchString = "OM_INPUT_IMG_DIR";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString != "false") {
		  	this->camInputImgDir = foundString;
		  	this->cameras.useCameras = false;
		}
		if(printParams)
	    {
  		    cout << "OM_INPUT_IMG_DIR " << foundString << endl;
	    }
	  }
	  searchString.clear();
	  found = string::npos;

	  // start image
	  searchString = "OM_INPUT_START_IMG";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->camInputStartImg =  atoi(foundString.c_str());
		  if(printParams)
	      {
  		      cout << "OM_INPUT_START_IMG " << this->camInputStartImg << endl;
	      }
	  }
	  searchString.clear();
	  found = string::npos;

	  // final image
	  searchString = "OM_INPUT_FINAL_IMG";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->camInputFinalImg = atoi(foundString.c_str());
		  if(printParams)
	      {
  		      cout << "OM_INPUT_FINAL_IMG " << this->camInputFinalImg << endl;
	      }
	  }
	  searchString.clear();
	  found = string::npos;

	  // reduceImageSizeTo320x240
	  searchString = "REDUCE_IMAGE_SIZE_TO_320X240";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  if (foundString == "true")
			  this->cameras.reduceImageSizeTo320x240 = true;
		  if (foundString == "false")
			  this->cameras.reduceImageSizeTo320x240 = false;
		  if(printParams)
	      {
  		      cout << "REDUCE_IMAGE_SIZE_TO_320X240 " << foundString << endl;
	      }
	  }
	  searchString.clear();
	  found = string::npos;

	  // automatic image storage
	  searchString = "AUTOMATIC_IMAGE_STORAGE_OM";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  if (foundString == "true")
			  this->datastorage.autoImageStorage = true;
		  if (foundString == "false")
			  this->datastorage.autoImageStorage = false;
		  if(printParams)
	      {
  		      cout << "AUTOMATIC_IMAGE_STORAGE_OM " << foundString << endl;
	      }
	  }
	  searchString.clear();
	  found = string::npos;

	  // unrectified image storage
	  searchString = "UNRECTIFIED_IMAGE_STORAGE_OM";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  if (foundString == "true")
			  this->datastorage.unrectifiedImageStorage = true;
		  if (foundString == "false")
			  this->datastorage.unrectifiedImageStorage = false;
		  if(printParams)
	      {
  		      cout << "UNRECTIFIED_IMAGE_STORAGE_OM " << foundString << endl;
	      }
	  }
	  searchString.clear();
	  found = string::npos;
}

void opticsmountGSP::parseOMCameraFile(){

	  string line;
	  string foundString;
	  size_t found;
	  //Open the /opt/GogglesDaemon/CAMERA_FILE
	  ifstream cameraFile(CAMERA_FILE);
	  if (cameraFile.is_open()) 
	  {
		  for (int count = 0; count < 16; count++) 
		  {
			  getline(cameraFile,line);
			  if (cameraFile.eof()) 
			  {
				  cameras.OPTICS_ID[count] = -1;
				  goto close;
			  }
			  found = line.find(":");
			  strcpy(cameras.LEFT_CAM_SERIAL[count],line.substr(0,found).c_str());
			  cameras.OPTICS_ID[count] = atoi(line.substr(found+1,string::npos).c_str());

			  cout << "Cam S/N & ID: " << cameras.LEFT_CAM_SERIAL[count] << ":" << cameras.OPTICS_ID[count] << endl;
		  }
	  }
	  else 
	  {
		  cout << "Unable to open: " << CAMERA_FILE << endl;
	  }
	  close:
	  {
		  cameraFile.close();
	  }
		
}
// Copied to haloGSP and changed to dataStorage_thread()
void opticsmountGSP::imageStorage_thread() {

	while(!stopImageStorageThread)
	{
		if (!storageImg_deque.empty())
		{
			struct timespec timeA, timeB;
			clock_gettime(CLOCK_REALTIME, &timeA);
			int deque_size = storageImg_deque.size();

			pthread_mutex_lock(&storageDeque_mutex);
			timestampedImage& timg = storageImg_deque.back();
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
				std::cout << "Deque Size: " << deque_size << ". Timestamp: " << timg.getTimestamp() << "Write time: " << dt << std::endl;
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

void opticsmountGSP::init()
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

	sprintf(this->calibParamDir, "/opt/GogglesOptics/Calib_Params/%s", this->calibParamSetName);

/////////////////// Camera Initialization section

	if (cameras.useCameras)
	{
		// Initialize 2 cameras
		GSPretVal = cameras.initTwoCameras(); // Errors handled within functions
		if (GSPretVal != 0)
		{
			printf("Cameras could not be initialized!\n");
			return exit(1);
		}
		// Start 2 cameras
		GSPretVal = cameras.startTwoCameras(); // Errors handled within functions
		if (GSPretVal != 0)
		{
			printf("Cameras could not be started!\n");
			return exit(1);
		}
	}

///////////////////

/////////////////// Image Rectification Initialization

	GSPretVal = rectifier.calcRectificationMaps(cameras.getImageWidth(), cameras.getImageHeight(), this->calibParamDir);
	if (GSPretVal != 0)
	{
		cout << "Rectification maps could not be processed!" << endl;
	}

/////////////////// Data Storage Initialization

	datastorage.initDataStorage(this->opticsmountName, this->camera_ID, this->runPath, this->cameras.getImageWidth(), this->cameras.getImageHeight());
	if (datastorage.autoImageStorage) {

		// create image-datastorage thread
		GSPretVal = pthread_create(&ImageStorageThread, NULL, this->imageStorage_thread_Helper, this);
		if (GSPretVal != 0)
		{
			printf("pthread_create ImageStorageThread failed\n");
			return exit(1);
		}

	}
///////////////////

///////////////////  Initialization of OpenCV Mats with correct Size (depending on cameras.reduceImageSizeTo320x240)

	leftImage.create(cameras.getImageHeight(), cameras.getImageWidth(), CV_8UC1);
	rightImage.create(cameras.getImageHeight(), cameras.getImageWidth(), CV_8UC1);

///////////////////

}

void opticsmountGSP::captureAndRectifyImages(cv::Mat& leftImg, cv::Mat& rightImg)
{

	cout << "captureAndRectifyImages entered" << endl;

	cv::Mat combined_img;

	if (cameras.useCameras)
	{
		GSPretVal = cameras.captureTwoImages(leftImg, rightImg, &cameras.leftImgNum, &cameras.rightImgNum, this->synchCheckFlag);
		if (GSPretVal != 0)
		{
			printf("Capture Two Images Failed\n");
		}

		this->updateTime();

		this->prevImageTimeStamp = this->imageTimeStamp;
		this->imageTimeStamp = this->testtime;
		double deltaTime = imageTimeStamp - prevImageTimeStamp;

		if (deltaTime > 2*(1000.0 / this->cameras.getFrameRate())) {
///			std::cout << "Lrg DeltaTime: " << deltaTime << "at: " << imageTimeStamp << std::endl;
		}

/*
		if(this->datastorage.unrectifiedImageStorage) {
			pthread_mutex_lock(&this->datastorage.storage_mutex);
			leftImage.copyTo(this->datastorage.leftStorageImgUnrect);
			rightImage.copyTo(this->datastorage.rightStorageImgUnrect);
			pthread_mutex_unlock(&this->datastorage.storage_mutex);
		}
*/
		// rectify images, but only if this->synchCheckFlag == 0, because in this case previous image frames are used, which are already rectified
		if (this->synchCheckFlag == 0 && rectifier.rectifierOn)
		{
			GSPretVal = rectifier.rectifyImages(leftImg, rightImg);
			if (GSPretVal != 0)
			{
				printf("Image rectification failed \n");
			}
		}

		if (videostreaming.videoStreamingOn) 
		{
			//is this code needed if its not being used??
			/*
			cv::Mat smallLeft, smallRight;
			cv::pyrDown(leftImg, smallLeft, cv::Size(leftImg.cols/2, leftImg.rows/2));
			cv::pyrDown(rightImg, smallRight, cv::Size(rightImg.cols/2, rightImg.rows/2));

			//place two images side by side
			combined_img.create( cv::Size(smallLeft.cols + smallRight.cols, smallLeft.rows > smallRight.rows ? smallLeft.rows : smallRight.rows),
					CV_MAKETYPE(smallLeft.depth(), 3) );
			cv::Mat imgLeft = combined_img( cv::Rect(0, 0, smallLeft.cols, smallLeft.rows) );
			cv::Mat imgRight = combined_img( cv::Rect(smallLeft.cols, 0, smallRight.cols, smallRight.rows) );

			cv::cvtColor(smallLeft, imgLeft, CV_GRAY2BGR );
			cv::cvtColor(smallRight, imgRight, CV_GRAY2BGR );

			// update default VideoBuffer with recitifed stereo image pair
			printf("Combined Image Size: [%d, %d]\n", combined_img.rows, combined_img.cols);
			videostreaming.update_MatVideoBuffer(defaultVideoBuffer, combined_img);
			*/
		}
	} 

	else 
	{

		stringstream currFileName;
		cv::Mat currCombinedImg;
		double imgTimestep;

		imgTimestep = 1.0E6 / this->cameras.getFrameRate();
		this->currentTime;
		usleep(imgTimestep);

		if(this->camInputImgCounter < this->camInputStartImg) 
		{
			this->camInputImgCounter = this->camInputStartImg;
		}

		currFileName.str("");
		currFileName << this->camInputImgDir << "/LeftRight" << this->camInputImgCounter << ".bmp";

		//end if less than 0 or if greater than specified number
		if(this->camInputFinalImg > 0 && this->camInputImgCounter > this->camInputFinalImg) 
		{
			std::cout << "Last Image read: " << this->camInputImgCounter << std::endl;
			this->shutdownCriterion = true;
		}

		//std::cout << "Reading Image: " << currFileName.str() << std::endl;

		currCombinedImg = cv::imread(currFileName.str());

		if (currCombinedImg.empty()) 
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
				currFileName << this->camInputImgDir << "/LeftRight" << this->camInputImgCounter << ".bmp";

				std::cout << "Re-Reading Image: " << currFileName.str() << std::endl;
				currCombinedImg = cv::imread(currFileName.str());

				if (currCombinedImg.empty()) 
				{
					std::cout << "Two Empty Images, shutting down: " << currFileName.str();
					this->shutdownCriterion = true;
					return;
				}
			}

		}

        cvtColor(currCombinedImg, currCombinedImg, CV_BGR2GRAY);
		cv::Mat imgLeft = currCombinedImg(cv::Rect(0, 0, leftImg.cols, leftImg.rows));
		cv::Mat imgRight = currCombinedImg(cv::Rect(leftImg.cols, 0, rightImg.cols, rightImg.rows));
		imgLeft.copyTo(leftImg);
		imgRight.copyTo(rightImg);

		currFileName.str("");
		this->camInputImgCounter++;

		this->updateTime();

		this->prevImageTimeStamp = this->imageTimeStamp;
		this->imageTimeStamp = this->testtime;
		double deltaTime = imageTimeStamp - prevImageTimeStamp;

		if (deltaTime > 2*(1000.0 / this->cameras.getFrameRate())) 
		{
//			std::cout << "Lrg DeltaTime: " << deltaTime << "at: " << imageTimeStamp << std::endl;
		}

	}

	struct timespec timeA, timeB;
	clock_gettime(CLOCK_REALTIME, &timeA);

	timestampedImage tImage(this->imageTimeStamp, leftImage, rightImage);

	pthread_mutex_lock(&storageDeque_mutex);
	storageImg_deque.push_front(tImage);
	pthread_mutex_unlock(&storageDeque_mutex);

	clock_gettime(CLOCK_REALTIME, &timeB);
	double save_time_diff = timeDiff(&timeA, &timeB);
	//std::cout << "Time to add to deque: " << save_time_diff << std::endl;
}

void opticsmountGSP::shutdown() 
{

	if (cameras.useCameras)
	{
		try
		{
			GSPretVal = cameras.stopTwoCameras();
			if (GSPretVal != 0)
				throw "Cameras could not be stopped!";
		}
		catch (char * str) 
		{
			cout << "Exception raised: " << str << '\n';
		}

		try
		{
			GSPretVal = cameras.closeTwoCameras();
			if (GSPretVal != 0)
				throw "Cameras could not be closed!";
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
}
