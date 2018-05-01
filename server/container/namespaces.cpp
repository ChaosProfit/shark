/*
 * namespaces.cpp
 *
 *  Created on: Mar 3, 2018
 *      Author: luguanglong
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#include <fstream>

#include "utils/log.hpp"

static int fileUpdate(char *filePath, char *value) {
	int idFd = open(filePath, O_RDWR);
	if (idFd < 0) {
		sharkLog(SHARK_LOG_DEBUG, "%s open failed\n", filePath);
		return -1;
	}

	snprintf(value, 128, "0 %d 1", getuid());

	int ret = write(idFd, value, strlen(value));
	if ((unsigned)ret != strlen(value)) {
		sharkLog(SHARK_LOG_DEBUG, "%s write failed\n", filePath);
		return -1;
	}

	close(idFd);

	sharkLog(SHARK_LOG_DEBUG, "%s file update %s successfully\n", filePath, value);
	return 0;
}

int userNsInit(int pid) {
	int ret = 0;
	char filePath[128] =  {0};
	char mapping[128] =  {0};

	snprintf(filePath, 128, "/proc/%d/uid_map", pid);
	snprintf(mapping, 128, "0 %d 1", getuid());
	ret = fileUpdate(filePath, mapping);

	snprintf(filePath, 128, "/proc/%d/gid_map", pid);
	snprintf(mapping, 128, "0 %d 1", getgid());
	ret = fileUpdate(filePath, mapping);

	sharkLog(SHARK_LOG_DEBUG, "userNsInit successfully, ret:%d\n", ret);
	return 0;
}

int utsNsInit(const char *hostname) {
	char tmpBuf[64] =  {0};
	int ret = 0;

	if (strlen(hostname) > 64) {
		sharkLog(SHARK_LOG_DEBUG, "lenth %d of %s is too long, limit is 64bytes\n", strlen(hostname), hostname);
		return -1;
	}

	ret = sethostname(hostname, 64);
	if (ret < 0) {
		sharkLog(SHARK_LOG_DEBUG, "sethostname %s failed, errno:%d\n", hostname, errno);
		return -1;
	}

	ret = gethostname(tmpBuf, 64);
	if (ret < 0) {
		sharkLog(SHARK_LOG_DEBUG, "gethostname %s failed, errno:%d\n", hostname, errno);
		return -1;
	}

	sharkLog(SHARK_LOG_DEBUG, "hostname:%s has been set successfully\n", tmpBuf);
	return 0;
}
