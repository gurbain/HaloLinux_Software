#include "testproject.h"

testproject::testproject () 
{
	maneuverNumber = 1;
	pthread_mutex_init(&keymutex, NULL);
	save_dir = "";
	isOrfInit= false;
	isThermoInit = false;
}

void testproject::GSinit()
{
 	HIDS opticscam = 1;
 	//opticsmount1 = new opticsmountGSP(this, opticscam);
	orf1 = new orfGSP(this);
	thermocam1 = new thermocamGSP(this);
}

void testproject::GSsetup()
{
	//opticsmount1->init();
	int retVal = thermocam1->init();
	if (retVal == 0)
		isThermoInit = true;
	retVal = orf1->init();
	if (retVal == 0)
		isOrfInit = true;
	
	// Create a new timestamp file and dir
	time_t rawtime;
	struct tm * timeinfo;
	char buffer1[100];
	stringstream buffer2;
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer1,100,"%d-%m-%Y-%I-%M", timeinfo);
	buffer2<<save_dir<<"/"<<buffer1;
	string dir = buffer2.str();
	struct stat st;
	if(stat(this->save_dir.c_str(),&st) != 0) {
		INFO<<"Creation of the folder "<<dir<<endl;
		mkdir(this->save_dir.c_str(), 0777);
		mkdir(dir.c_str(), 0777);
	}
	this->save_dir = dir;
	stringstream ss;
	ss<<this->save_dir<<"/timestampOpticsMount.txt";
	tsfile.open(ss.str().c_str());
	cout<<ss.str().c_str()<<endl;
	ss.str("");
	if (tsfile.is_open())
		tsfile<<endl<<endl<<"######################### NEW SESSION #######################"<<endl<<endl;
	
	if (isOrfInit)
		orf1->captureAndRectify(orf1->depth, orf1->visual, orf1->confidency);
	//opticsmount1->captureAndRectifyImages(opticsmount1->leftImage, opticsmount1->rightImage);
	if (isThermoInit)
		thermocam1->capture(thermocam1->irImg);
}

void testproject::GSrunMain()
{
	// Take Optics mount images
	//ts.start();
	//opticsmount1->captureAndRectifyImages(opticsmount1->leftImage, opticsmount1->rightImage);
	//ts.stop();
	
	// Take Orf images
	if (isOrfInit)
		orf1->captureAndSaveRectified();
	
	// Take Thermo images
	if (isThermoInit)
		thermocam1->captureAndSave();
	
	// Save Optics mount images
	//this->GSsaveOpticsMount(opticsmount1->leftImage, opticsmount1->rightImage);
	
	// Sleep 1 second
	usleep(800000);
}

void testproject::GSsaveOpticsMount(cv::Mat& leftImg, cv::Mat& rightImg)
{
	//Save jpg images
	stringstream ssl, ssr;
	ssl<<this->save_dir<<"/leftImg"<<imgNum<<".png";
	ssr<<this->save_dir<<"/rightImg"<<imgNum<<".png";
	string filenamel = ssl.str();
	string filenamer = ssr.str();
	ssl.str("");
	ssr.str("");
	
	try {
		imwrite(filenamel, leftImg);
		imwrite(filenamer, rightImg);
	} catch (int ex) {
		cout<<"Exception converting image to jpg format: "<<ex<<endl;
		return;
	}
	
	// Save time stamp
	if (!tsfile.is_open()) {
		tsfile.open(this->timestamps.c_str());
		tsfile<<endl<<endl<<"######################### NEW SESSION #######################"<<endl<<endl;
		if (!tsfile.is_open()) {
			cout<<"Impossible to open the file"<<endl;
			return;
		}
	}
	if (imgNum==0)
		cout<<"Saving Opticsmount images into folder "<<this->save_dir<<endl;
	tsfile<<"IMAGENUM\t"<<imgNum<<"\tPROCTIME\t"<<ts.getProcTime()<<"\tMEANTIME\t"<<ts.getMeanTime()<<"\tDIFF\t"<<ts.getMeanTime()-tslast<<endl;

	imgNum++;
	tslast = ts.getMeanTime();
}


void testproject::GSbackgroundTask()
{
	// Nothing to do here
}

void testproject::GSparseCommandlineArgs(int argc, char *argv[])
{
	if (argc > 4) {
		sprintf(this->calibParamSetName, "%s", argv[3]);
	}
}

void testproject::GSparseParameterFile(string line) 
{
	// Create variables
	string searchString;
	string foundString;
	size_t found;
	
	// Use the parse function of ORF and opticsmounts
	orf1->parseParameterFile(line, true);
	//opticsmount1->parseParameterFile(line, false);
	thermocam1->parseParameterFile(line, true);
	
	// Parse the Save dir name
	searchString = "ORF_SAVE_DIR";
	found = line.find(searchString);
	if (found != string::npos) {
		foundString = line.substr( found+searchString.size()+1, string::npos );
		this->save_dir =  foundString.c_str();
	}
	searchString.clear();
	found = string::npos;

}

void testproject::GScleanup() 
{
	if (isOrfInit)
		orf1->shutdown();
	//opticsmount1->shutdown();
	if (isThermoInit)
		thermocam1->shutdown();
	
	delete orf1;
	//delete opticsmount1;
	delete thermocam1;
}
