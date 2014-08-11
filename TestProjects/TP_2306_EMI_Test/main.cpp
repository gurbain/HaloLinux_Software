/* This test project is for the purpose of EMI testing in June 2014. 
   It will demonstrate functionality of two docking port cameras and one set of optics mount cameras.
   The code is essentially a combination of test projects 2305 and 2302.
   2 docking port objects and 1 optics mount object are created in the code, then the corresponding image collection and processing methods are called through GSRunMain.
*/

#include "testproject.h"

#include <iostream>

using namespace std;

testproject *test = NULL;

void terminateHandlerHelper(int sig)
{
	test->terminateHandler(sig);
}

void preInit()
{
	test = new testproject;

	signal(SIGTERM, terminateHandlerHelper);

	signal(SIGABRT, terminateHandlerHelper);
	signal(SIGINT, terminateHandlerHelper);

}



int main (int argc, char *argv[])
{
	preInit();

	test->runMain(argc, argv);

	delete test;
	return 0;

}




