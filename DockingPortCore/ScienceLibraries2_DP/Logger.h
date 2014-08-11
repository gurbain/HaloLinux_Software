/*
 * Logger.h
 *
 *  Created on: May 2, 2012
 *      Author: muggler
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <iostream>
#include <fstream>
#include <string>

class Logger {
public:
	Logger();
	virtual ~Logger();

	void log(std::string message, bool appendNewLine=true);
	void newLine();

	void setStreamToFile(const std::string filename, bool stream=true);
	void setStreamToStdout(bool stream=true);

private:
	bool streamToStdout;
	bool streamToFile;

	std::ofstream logFile;
};

#endif /* LOGGER_H_ */
