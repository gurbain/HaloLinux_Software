#include "dockingportGSP.h"

// Moved commented parts to haloGSP
void dockingportGSP::parseParameterFile(string line) {

	  string searchString;
	  string foundString;
	  size_t found;
	  // frameRate
	  searchString = "FRAME_RATE";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->camera.setFrameRate( atof(foundString.c_str()) );
		  cout << "FRAME_RATE " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  // exposureTime
	  searchString = "EXPOSURE_TIME";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->camera.setExposureTime( atoi(foundString.c_str()) );
		  cout << "EXPOSURE_TIME " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  // exposureTime
	  searchString = "HW_GAIN";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->camera.setHWGain( atoi(foundString.c_str()) );
		  cout << "HW_GAIN " << foundString << endl;
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
			  this->camera.useSynchCams = true;
		  if (foundString == "false")
			  this->camera.useSynchCams = false;
		  cout << "USE_SYNCH_CAMS " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  // use images from file
	  searchString = "CAM_INPUT_IMG_DIR";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString != "false") {
		  	this->camInputImgDir = foundString;
		  	this->camera.useCamera = false;
		  	cout << "CAM_INPUT_IMG_DIR " << foundString << endl;
		}
	  }
	  searchString.clear();
	  found = string::npos;

	  // start image
	  searchString = "CAM_INPUT_START_IMG";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->camInputStartImg =  atoi(foundString.c_str());
		  cout << "CAM_INPUT_START_IMG " << this->camInputStartImg << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  // start image
	  searchString = "CAM_INPUT_FINAL_IMG";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  this->camInputFinalImg = atoi(foundString.c_str());
		  cout << "CAM_INPUT_FINAL_IMG " << this->camInputFinalImg << endl;
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
			  this->camera.reduceImageSizeTo320x240 = true;
		  if (foundString == "false")
			  this->camera.reduceImageSizeTo320x240 = false;
		  cout << "REDUCE_IMAGE_SIZE_TO_320X240 " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;


	  // autoImageStorage
	  searchString = "AUTOMATIC_IMAGE_STORAGE";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  if (foundString == "true")
			  this->datastorage.autoImageStorage = true;
		  if (foundString == "false")
			  this->datastorage.autoImageStorage = false;
		  cout << "AUTOMATIC_IMAGE_STORAGE " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  // autoImageStorage
	  searchString = "UNRECTIFIED_IMAGE_STORAGE";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		  foundString = line.substr( found+searchString.size()+1, string::npos );
		  if (foundString == "true")
			  this->datastorage.unrectifiedImageStorage = true;
		  if (foundString == "false")
			  this->datastorage.unrectifiedImageStorage = false;
		  cout << "UNRECTIFIED_IMAGE_STORAGE " << foundString << endl;
	  }
	  searchString.clear();
	  found = string::npos;

	  // useOneCamera
	  searchString = "ONE_CAM_XS";
	  found = line.find(searchString);
	  if (found != string::npos)
	  {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		if (foundString == "true") {
		  	this->camera.useOneCamera = true;
		  	cout << "ONE_CAM_XS " << foundString << endl;
		}
	  }
	  searchString.clear();
	  found = string::npos;

	  //other files on the filesystem

	  //Open the /opt/GogglesDaemon/CAMERA_FILE
	  ifstream cameraFile(CAMERA_FILE);

	  if (cameraFile.is_open()) {
		  for (int count = 0; count < 16; count++) {
			  getline(cameraFile,line);
			  if (cameraFile.eof()) {
				  camera.OPTICS_ID[count] = -1;
				  break;
			  }
			  found = line.find(":");
			  strcpy(camera.SINGLE_CAM_SERIAL[count],line.substr(0,found).c_str());
			  camera.OPTICS_ID[count] = atoi(line.substr(found+1,string::npos).c_str());

			  cout << "Cam S/N & ID: " << camera.SINGLE_CAM_SERIAL[count] << ":" << camera.OPTICS_ID[count] << endl;
		  }
	  } else {
		  cout << "Unable to open: " << CAMERA_FILE << endl;
	  }

}

// Copied to haloGSP and changed to dataStorage_thread()
void dockingportGSP::imageStorage_thread() {

	//while(shutdownCriterion == false)
	//while(!shutdownCriterion || storageImg_deque.size() > 0)
	while(!stopImageStorageThread)
	{
		if (!storageImg_deque.empty()) {
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
			if (deque_size > 1) {
				std::cout << "Deque Size: " << deque_size << ". Timestamp: " << timg.getTimestamp() << "Write time: " << dt << std::endl;
			}
			//std::cout << "Deque Size: " << deque_size << ". Timestamp: " << timg.getTimestamp() << "Write time: " << dt << std::endl;

		} else {
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

	//sprintf(this->calibParamDir, "%s/Calib_Params/%s", this->HPFpath, this->calibParamSetName);
	sprintf(this->calibParamDir, "/opt/GogglesOptics/Calib_Params/%s", this->calibParamSetName);

///////////////////

///////////////////	Camera Initialization section

	// Initialize camera
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

///////////////////

/////////////////// Image Rectification Initialization

	GSPretVal = rectifier.calcRectificationMaps(camera.getImageWidth(), camera.getImageHeight(), this->calibParamDir);
	if (GSPretVal != 0)
	{
		cout << "Rectification maps could not be processed!" << endl;
	}

///////////////////

///////////////////	Data Storage Initialization
	datastorage.initDataStorage(this->testname, this->HPFpath, this->camera.getImageWidth(), this->camera.getImageHeight());
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

///////////////////  Initialization of OpenCV Mats with correct Size (depending on camera.reduceImageSizeTo320x240)


	singleImage.create(camera.getImageHeight(), camera.getImageWidth(), CV_8UC3);

///////////////////

}


void dockingportGSP::captureAndRectifySingleImage(cv::Mat& singleImg)
{

	cout << "captureAndRectifySingleImage entered"  << endl;

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

			if (deltaTime > 2*(1000.0 / this->camera.getFrameRate())) {
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

	/* Duncan comment out. looks complicated, unneeded without two imgs
			// rectify images, but only if this->synchCheckFlag == 0, because in this case previous image frames are used, which are already rectified
			if (this->synchCheckFlag == 0 && rectifier.rectifierOn)
			{
				GSPretVal = rectifier.rectifyImages(leftImg, rightImg);
				if (GSPretVal != 0)
				{
					printf("Image rectification failed \n");
				}
			}
	*/
		} else {

			stringstream currFileName;
			cv::Mat currCombinedImg;
			double imgTimestep;

			imgTimestep = 1.0E6 / this->camera.getFrameRate();
			this->currentTime;
			usleep(imgTimestep);

			if(this->camInputImgCounter < this->camInputStartImg) {
				this->camInputImgCounter = this->camInputStartImg;
			}

			currFileName.str("");
			currFileName << this->camInputImgDir << "/SingleImage" << this->camInputImgCounter << ".bmp";

			//end if less than 0 or if greater than specified number
			if(this->camInputFinalImg > 0 && this->camInputImgCounter > this->camInputFinalImg) {
				std::cout << "Last Image read: " << this->camInputImgCounter << std::endl;
				this->shutdownCriterion = true;
			}

			//std::cout << "Reading Image: " << currFileName.str() << std::endl;

			singleImg = cv::imread(currFileName.str());

			if (singleImg.empty()) {
				std::cout << "Empty Image: " << currFileName.str() << std::endl;

				if (this->camInputFinalImg == -1) {
					this->shutdownCriterion = true;
				} else if (this->camInputFinalImg == -10) {
					this->camInputImgCounter = this->camInputStartImg;
					currFileName.str("");
					currFileName << this->camInputImgDir << "/SingleImage" << this->camInputImgCounter << ".bmp";

					std::cout << "Re-Reading Image: " << currFileName.str() << std::endl;
					singleImg = cv::imread(currFileName.str());

					if (singleImg.empty()) {
						std::cout << "Two Empty Images, shutting down: " << currFileName.str();
						this->shutdownCriterion = true;
						return;
					}
				}

			}

//	        cvtColor( singleImg, singleImg, CV_BGR2GRAY );


			currFileName.str("");
			this->camInputImgCounter++;

			this->updateTime();

			this->prevImageTimeStamp = this->imageTimeStamp;
			this->imageTimeStamp = this->testtime;
			double deltaTime = imageTimeStamp - prevImageTimeStamp;

			if (deltaTime > 2*(1000.0 / this->camera.getFrameRate())) {
	//			std::cout << "Lrg DeltaTime: " << deltaTime << "at: " << imageTimeStamp << std::endl;
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

void dockingportGSP::GSprocessSingleImage(cv::Mat& singleImg){

	cout << "Single image processing algorithms go here" << endl;

}

void dockingportGSP::shutdown() {

	if (camera.useCamera)
	{
		try{
			GSPretVal = camera.stopOneCamera();
			if (GSPretVal != 0)
				throw "Camera could not be stopped!";
		}
		catch (char * str) {
			cout << "Exception raised: " << str << '\n';
		}

		try{
			GSPretVal = camera.closeOneCamera();
			if (GSPretVal != 0)
				throw "Camera could not be closed!";
		}
		catch (char * str) {
			cout << "Exception raised: " << str << '\n';
		}

		sleep(2);

		// shut down data storage
		if(datastorage.autoImageStorage) {
			int storimgshutcnt = 0;
			int timeout_delay = 500000;
			//wait for storage image deque to clear
			while(storageImg_deque.size() > 0) {
				if (storimgshutcnt++ > (20*1000000 / timeout_delay)) {
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

