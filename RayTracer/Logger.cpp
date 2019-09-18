#include "Logger.h"
#include <ctime>
#include <fstream>

Logger::Logger() {
	toFile = false;
}

Logger::Logger(string fileName) {
	toFile = true;
	file.open(fileName);
	assertFileExist(fileName);
}

Logger::~Logger() {
	if (toFile) file.close();
}

void Logger::info(string msg) {
	if (level == NONE) return;
	time_t now = time(0);
	char dt[26]; ctime_s(dt, sizeof dt, &now);
	string message = "INFO: " + string(dt) + msg + '\n';
	if (toFile) file << message;
	else cout << message;
}

void Logger::warn(string msg) {
	if (level == NONE || level == INFO) return;
	time_t now = time(0);
	char dt[26]; ctime_s(dt, sizeof dt, &now);
	string message = "WARN: " + string(dt) + msg + '\n';
	if (toFile) file << message;
	else cout << message;
}

void Logger::error(string msg) {
	if (level != ERROR) return;
	time_t now = time(0);
	char dt[26]; ctime_s(dt, sizeof dt, &now);
	string message = "ERROR: " + string(dt) + msg + '\n';
	if (toFile) file << message;
	else cout << message;
}

void Logger::setLoggerLevel(LOGGER_LEVEL mLevel) {
	level = mLevel;
}