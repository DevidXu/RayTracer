#include "Logger.h"
#include <ctime>
#include <chrono>
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

const std::string currentDateTime() {
	time_t     now = time(0);
	return to_string(now);
}

string Logger::getFormattedTime() {
	string ts = currentDateTime(); ts = trim(ts);
	return ts.substr(0, ts.size() - 5) + "  ";
}

void Logger::info(string msg) {
	if (level == NONE) return;
	string message = "INFO: " + getFormattedTime() + msg + '\n';
	if (toFile) file << message;
	else cout << message;
}

void Logger::warn(string msg) {
	if (level == NONE || level == INFO) return;
	string message = "WARN: " + getFormattedTime() + msg + '\n';
	if (toFile) file << message;
	else cout << message;
}

void Logger::error(string msg) {
	if (level != ERROR) return;
	string message = "ERROR: " + getFormattedTime() + msg + '\n';
	if (toFile) file << message;
	else cout << message;
}

void Logger::setLoggerLevel(LOGGER_LEVEL mLevel) {
	level = mLevel;
}