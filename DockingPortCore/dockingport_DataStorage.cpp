#include "dockingport_DataStorage.h"

void dockingport_DataStorage::makeDirectory(char newDirectory[200]) 
{
	int dummy2;
	dummy2 = mkdir(newDirectory, 0777); //makes new directory with full r/w permissions to user
}

void dockingport_DataStorage::initDataStorage(char _dockportName[200], int _camera_ID, char _runPath[200], int imgWidth, int imgHeight) 
{
	char newDirectory[200];
	char newSubdirectory[200];

	pthread_mutex_init(&this->storage_mutex, NULL);
	cv::Size size(imgWidth*2 , imgHeight);
	this->storageImg.create( size, CV_8UC1 );
	this->singleStorageImg = this->storageImg( cv::Rect(0, 0, imgWidth, imgHeight) );
	this->storageImgUnrect.create( size, CV_8UC1 );
	this->singleStorageImgUnrect = this->storageImgUnrect( cv::Rect(0, 0, imgWidth, imgHeight) );

	// make newDirectory equal that of the runPath
	sprintf(newDirectory, "%s", _runPath); 
    DIR *resultsDir;

	// make subfolder for Guest Scientist data storage
	sprintf(newSubdirectory,"%s/GSdata", newDirectory);
	resultsDir = opendir (newDirectory);
	this->makeDirectory(newSubdirectory);
	sprintf(this->GSstoragepath, "%s", newSubdirectory);

	// make images subfolder
	sprintf(newSubdirectory,"%s/%s_%d_Images", newDirectory, _dockportName, _camera_ID); //RS added dockport and cam ID
	resultsDir = opendir (newDirectory);
	this->makeDirectory(newSubdirectory);
	sprintf(this->imagestoragepath, "%s", newSubdirectory);

	// open file for image timetags
	sprintf(timetagStorageFileName, "%s/imageTimetags.csv", newSubdirectory);
	timetagStorageFile.open(timetagStorageFileName);
}

void dockingport_DataStorage::saveTimestampedImages(double imagetimestamp) 
{
	printf("Dockingport::saveTimestampedImages entered");

	// write time stamp together with current image number in file
	timetagStorageFile << numsaved << "," << imagetimestamp << "\n";

	// store images in Results folder
	sprintf(this->singleImageStorageName, "%s/SingleImage%d.bmp", this->imagestoragepath, numsaved);
	cv::imwrite(this->singleImageStorageName, this->storageImg);

	if(this->unrectifiedImageStorage) 
	{
		sprintf(this->singleImageStorageNameUnrect, "%s/SingleImageUnrect%d.bmp", this->imagestoragepath, numsaved);
		cv::imwrite(this->singleImageStorageNameUnrect, this->storageImgUnrect);
	}

	numsaved++;
}

void dockingport_DataStorage::shutdownDataStorage() 
{
	timetagStorageFile.close();
}

int dockingport_DataStorage::getNumSaved() 
{
	return numsaved;
}

string dockingport_DataStorage::getGSdatastoragePath() 
{
	return string( this->GSstoragepath) + "/";
}

string dockingport_DataStorage::getGPFpath() 
{
	return string(GPFpath) + "/";
}

string dockingport_DataStorage::getTimeOfTestStart() 
{
	return string(this->timeOfTeststart);
}

string dockingport_DataStorage::newGSDataDirectory(char newDirName[200]) 
{
	char newDir[200];
    DIR *dir;

	sprintf(newDir, "%s/%s",this->GSstoragepath, newDirName);
    dir = opendir (newDir);
    if (dir == NULL)
    {
		// make new RESULTS directory for whole Program = Group
    	this->makeDirectory(newDir);
    	printf("New GSdata Directory Created: %s\n\r", newDir);
    }
	return string(newDir) + "/";
}
