/*
 * common.cpp
 *
 *  Created on: 2018年2月25日
 *      Author: luguanglong
 */

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include <iostream>

int logOutput(const char *filePath, const char *format ,...)
{
	char argBuf[1024] = {0};
	char retBuf[256] = {0};

	va_list argLst;
	va_start(argLst,format);
	vsnprintf(argBuf, 1024, format, argLst);
	va_end(argLst);

	int fd = open(filePath, O_CREAT|O_RDWR|O_APPEND);
	if(fd < 0){
		std::cout << "Open file Failed\n";
		return -1;
	}

	int ret = write(fd, argBuf, strlen(argBuf));
	if(ret < 0){
		std::cout << "Write " << filePath << " failed\n";
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}
