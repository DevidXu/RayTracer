#pragma once
#ifndef _LOGGER_H__
#define _LOGGER_H__

#include "Helper.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

enum LOGGER_LEVEL {NONE, INFO, WARN, ERROR};

class Logger: public Singleton<Logger> {
public:
	Logger();
	Logger(string fileName);
	~Logger();

	void info(string msg);
	void warn(string msg);
	void error(string msg);
	void setLoggerLevel(LOGGER_LEVEL mLevel);
private:
	bool toFile = false;
	ofstream file;
	LOGGER_LEVEL level;
};

#endif