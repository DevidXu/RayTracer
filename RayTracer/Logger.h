#pragma once
#ifndef _LOGGER_H__
#define _LOGGER_H__

#include <iostream>
#include <fstream>
#include <string>
#include "Helper.h"

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
	
	string getFormattedTime();
};

#define INFO(A) Logger::Instance()->info(A)
#define WARN(A) Logger::Instance()->warn(A)
#define ERROR(A) Logger::Instance()->error(A)

#endif