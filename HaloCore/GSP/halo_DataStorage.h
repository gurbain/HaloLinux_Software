#ifndef HALO_DATA_STORAGE_H
#define HALO_DATA_STORAGE_H

// C++ libraries
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <iostream> 
#include <stdlib.h> 

class halo_DataStorage
{
	char testname[100];
	char GPFpath[200];
	char timeOfTeststart[200];

public:

	pthread_mutex_t storage_mutex;

	halo_DataStorage() {
		//parameters set in peripheral datastorage
	}

	std::string initDataStorage(char _testname[100], char _testprojectpath[200]); //Returns TP/Results/run path to use in peripheral DataStorage
	void makeDirectory(char newDirectory[200]);
};

#endif
