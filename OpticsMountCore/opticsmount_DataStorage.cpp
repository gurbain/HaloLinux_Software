#include "opticsmount_DataStorage.h"

void opticsmount_DataStorage::makeDirectory(char newDirectory[200]) {
	int dummy2;
	dummy2 = mkdir(newDirectory, 0777); //makes new directory with full r/w permissions to user
}

void opticsmount_DataStorage::initDataStorage(char _opticsmountName[200], int _camera_ID, char _runPath[100], int imgWidth, int imgHeight) {

	char newDirectory[200];
	char newSubdirectory[200];

	pthread_mutex_init(&this->storage_mutex, NULL);

	cv::Size size(imgWidth*2 , imgHeight);
	this->storageImg.create( size, CV_8UC1 );
	this->leftStorageImg = this->storageImg( cv::Rect(0, 0, imgWidth, imgHeight) );
	this->rightStorageImg = this->storageImg( cv::Rect(imgWidth, 0, imgWidth, imgHeight) );

	this->storageImgUnrect.create( size, CV_8UC1 );
	this->leftStorageImgUnrect = this->storageImgUnrect( cv::Rect(0, 0, imgWidth, imgHeight) );
	this->rightStorageImgUnrect = this->storageImgUnrect( cv::Rect(imgWidth, 0, imgWidth, imgHeight) );

    // make newDirectory equal that of the runPath
	sprintf(newDirectory, "%s", _runPath); 
    DIR *resultsDir;

	// make subfolder for Guest Scientist data storage
	sprintf(newSubdirectory,"%s/GSdata", newDirectory);
	resultsDir = opendir (newDirectory);
	this->makeDirectory(newSubdirectory);
	sprintf(this->GSstoragepath, "%s", newSubdirectory);

	// make images subfolder
	sprintf(newSubdirectory,"%s/%s_%d_Images", newDirectory, _opticsmountName, _camera_ID); //RS added dockport and cam ID
	resultsDir = opendir (newDirectory);
	this->makeDirectory(newSubdirectory);
	sprintf(this->imagestoragepath, "%s", newSubdirectory);

	// open file for image timetags
	sprintf(timetagStorageFileName, "%s/imageTimetags.csv", newSubdirectory);
	timetagStorageFile.open(timetagStorageFileName);
}

void opticsmount_DataStorage::saveTimestampedImages(double imagetimestamp) {

	printf("OpticsMount::saveTimestampedImages entered");

	// write time stamp together with current image number in file
	timetagStorageFile << numsaved << "," << imagetimestamp << "\n";

	// store images in Results folder
//	sprintf(leftImageStorageName, "%s/Left%d.bmp", this->imagestoragepath, numsaved);
//	sprintf(rightImageStorageName, "%s/Right%d.bmp", this->imagestoragepath, numsaved);
//	cv::imwrite(leftImageStorageName, leftframe);
//	cv::imwrite(rightImageStorageName, rightframe);

	sprintf(this->leftRightImageStorageName, "%s/LeftRight%d.bmp", this->imagestoragepath, numsaved);
	cv::imwrite(this->leftRightImageStorageName, this->storageImg);

	if(this->unrectifiedImageStorage)
	{
		sprintf(this->leftRightImageStorageNameUnrect, "%s/LeftRightUnrect%d.bmp", this->imagestoragepath, numsaved);
		cv::imwrite(this->leftRightImageStorageNameUnrect, this->storageImgUnrect);
	}

	numsaved++;
}

void opticsmount_DataStorage::shutdownDataStorage() {

	timetagStorageFile.close();

}

int opticsmount_DataStorage::getNumSaved() {
	return numsaved;
}

string opticsmount_DataStorage::getGSdatastoragePath() {

	return string( this->GSstoragepath) + "/";
}

string opticsmount_DataStorage::getGPFpath() {

	return string( this->GPFpath) + "/";
}

string opticsmount_DataStorage::getTimeOfTestStart() {
	return string(this->timeOfTeststart);
}

string opticsmount_DataStorage::newGSDataDirectory(char newDirName[200]) {
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
