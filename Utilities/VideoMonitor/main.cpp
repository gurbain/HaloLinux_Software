#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <ctime>

#include "networkServer.h"

using namespace std;

int main (int argc, char *argv[])
{
	int i = 0;
	int retVal;
	NetworkServerClass networkServer;

	retVal = networkServer.init();
	if (retVal != 0) {
		printf("Init Error\n");
		return -1;
	}

	std::time_t start = std::time(NULL);
	while (i < 1000) {
		networkServer.receiveMessages();
		 std::cout << "Elapsed time: " << std::difftime(std::time(NULL), start) << " s.\n";
	}

	networkServer.closeServer();
	return 0;

}
