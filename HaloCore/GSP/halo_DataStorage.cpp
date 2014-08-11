#include "halo_DataStorage.h"

void halo_DataStorage::makeDirectory(char newDirectory[200]) 
{
	int dummy2;
	dummy2 = mkdir(newDirectory, 0777); //makes new directory with full r/w permissions to user
}

std::string halo_DataStorage::initDataStorage(char _testname[100], char _GPFpath[200]) 
{
	sprintf(testname, "%s", _testname);
	sprintf(GPFpath,"%s", _GPFpath);

	char newDirectory[200];
	char newDirectorySystemCmd[200];
	DIR *resultsDir;

	// check if general Results Directory exists, if not, create it
	sprintf(newDirectory, "%s/Results", GPFpath); 
    resultsDir = opendir (newDirectory);
    if (resultsDir == NULL)
    {
	// make new RESULTS directory for whole Program = Group
    	this->makeDirectory(newDirectory);
    }

	// check if Results Directory for current testproject exists, if not, create it
	sprintf(newDirectory, "%s/%s", newDirectory, testname); //HL
	resultsDir = opendir (newDirectory);
    if (resultsDir == NULL)
    {
		// make new RESULTS directory for currently running testproject
    	this->makeDirectory(newDirectory);
    }

	// get current UTC time for generating new results directory for currently running test, append "UDP" + camera_ID number at end
	time_t rawtime;
	struct tm * ptm;
	time ( &rawtime );
	ptm = gmtime ( &rawtime ); // gives the time in UTC
	sprintf (this->timeOfTeststart, "%04d_%02d_%02d_%02d_%02d_%02d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

	// make new directory first
	sprintf(newDirectory, "%s/run_%s", newDirectory, this->timeOfTeststart); //HL
	resultsDir = opendir (newDirectory);
    if (resultsDir == NULL)
    {
		// make new RUN directory for currently running testproject
    	this->makeDirectory(newDirectory);
    }

	return newDirectory;
}
