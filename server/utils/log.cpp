/*
 * log.cpp
 *
 *  Created on: 2018年2月3日
 *      Author: luguanglong
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include <string>

#include "log.hpp"

const static int SHARK_LOG_MODE = SHARK_LOG_DEBUG;
const static int LOG_BUF_SIZE = 2048;
static char LOG_FILE_PATH[128] =  {0};

void sharkPrint(int debugMode, char *file, int line, const char *format ,...) {
	static char outBuf[LOG_BUF_SIZE] =  {0};
	static char logLevel[12] =  {0};

	if (debugMode > SHARK_LOG_MODE) {
		return;
	}

	time_t rawTime = 0;
	time(&rawTime);
	char *timeStr = ctime(&rawTime);
	timeStr[strlen(timeStr)-1] = 0;

	switch(debugMode) {
	case SHARK_LOG_ERR:
		snprintf(logLevel, 12, "Error");
		break;
	case SHARK_LOG_INFO:
		snprintf(logLevel, 12, "Info");
		break;
	case SHARK_LOG_DEBUG:
		snprintf(logLevel, 12, "Debug");
		break;
	default:
		return;
	}

	int fd = open(LOG_FILE_PATH, O_CREAT|O_APPEND|O_RDWR, 00777);

	snprintf(outBuf, LOG_BUF_SIZE, "%s, %s, file:%s, line:%d:", logLevel, timeStr, file, line);
	write(fd, outBuf, strlen(outBuf));

	va_list argLst;
	va_start(argLst, format);
	vsnprintf(outBuf, LOG_BUF_SIZE, format, argLst);
	write(fd, outBuf, strlen(outBuf));
	va_end(argLst);

	close(fd);
	return;
}

void sharkPrint(int debugMode, char *file, int line, std::string &data) {
	sharkPrint(debugMode, file, line, data.data());
}

int sharkLogInit(char *log_file_path) {
	snprintf(LOG_FILE_PATH, 128, log_file_path);

	return 0;
}
