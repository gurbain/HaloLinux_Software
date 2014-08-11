/*
 * Logger.cpp
 *
 *  Created on: May 2, 2012
 *      Author: muggler
 */

#include "Logger.h"

Logger::Logger() {
	streamToStdout = false;
	streamToFile = false;
}

Logger::~Logger() {
	logFile.close();
}

void Logger::log(std::string message, bool appendNewLine) {
	if (streamToStdout) {
		std::cout << message;

		if (appendNewLine) {
			std::cout << std::endl;
		} else {
			std::cout.flush();
		}
	}

	if (streamToFile) {
		logFile << message;

		if (appendNewLine) {
			logFile << std::endl;
		}
	}
}

void Logger::newLine() {
	std::cout << std::endl;
}

void Logger::setStreamToFile(const std::string filename, bool stream) {
	streamToFile = stream;
	if (streamToFile) {
		logFile.open(filename.c_str());
	}
}

void Logger::setStreamToStdout(bool stream) {
	streamToStdout = stream;
}
