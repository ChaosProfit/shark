/*
 * interface.cpp
 *
 *  Created on: Jan 24, 2018
 *      Author: luguanglong
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include <memory>

#include "cliClient.hpp"
#include "utils/exceptions.hpp"
#include "utils/log.hpp"

shark::CliClient::CliClient() {
	rcvBuf = malloc(RCV_BUF_SIZE);
	if (rcvBuf == NULL) {
		sharkLog(SHARK_LOG_ERR, "rcv buf malloc failed\n");
		throw new SharkException("rcvBuf malloc failed");
	}
	memset(rcvBuf, 0, RCV_BUF_SIZE);

	sharkLog(SHARK_LOG_DEBUG, "initCommon successfully\n");
	return;
}

shark::CliClient::~CliClient() {
	if (rcvBuf != NULL) {
		free(rcvBuf);
	}

	sharkLog(SHARK_LOG_INFO, "Interface destruct successfully\n");
	return;
}

void* shark::CliClient::dataSend(const void *data, int dataSize) {
	int ret = 0;
	int comFd = 0;
	struct sockaddr_un addr;

	comFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (comFd < 0) {
		sharkLog(SHARK_LOG_ERR, "socket create failed\n");
		throw new SharkException("interfaceFd socket failed");
	}

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SHARK_INTERFACE_PATH, sizeof(addr.sun_path) - 1);

	ret = connect(comFd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		sharkLog(SHARK_LOG_ERR, "socket connect failed, errno:%d\n", errno);
		throw new SharkException("connect failed");
	}

	ret = send(comFd, data, dataSize, MSG_NOSIGNAL);
	if (ret != (signed int)dataSize) {
		sharkLog(SHARK_LOG_ERR, "cmd send failed\n");
		throw new SharkException("Json Command send failed");
	}

	sharkLog(SHARK_LOG_DEBUG, "cmd sent successfully\n");
	usleep(500000);

	memset(rcvBuf, 0, RCV_BUF_SIZE);
	ret = recv(comFd, rcvBuf, RCV_BUF_SIZE, MSG_CMSG_CLOEXEC);
	if (ret < 0) {
		sharkLog(SHARK_LOG_ERR, "recv failed,errno:%d\n", errno);
		throw new SharkException("received failed");
	}

	sharkLog(SHARK_LOG_DEBUG, "rcvLen:%d,rcv:%s\n", \
			 ret, (char *)rcvBuf);
	return rcvBuf;
}
