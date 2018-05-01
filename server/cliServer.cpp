/*
 * CliServer.cpp
 *
 *  Created on: Jan 24, 2018
 *      Author: luguanglong
 */

#include "cliServer.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#include <memory>

#include "container/containerPool.hpp"
#include "utils/exceptions.hpp"
#include "utils/log.hpp"
#include "utils/json.hpp"

shark::CliServer::CliServer(ContainerPool &cPool) :
cPool(cPool) {
	int ret = 0;

	unlink(SHARK_INTERFACE_PATH);
	interfaceFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (interfaceFd < 0) {
		sharkLog(SHARK_LOG_ERR, "interfaceFd socker failed\n");
		throw SharkException("interfaceFd socket failed");
	}

	sepoll.addPort(interfaceFd);

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SHARK_INTERFACE_PATH, sizeof(addr.sun_path) - 1);
	ret = bind(interfaceFd, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		sharkLog(SHARK_LOG_ERR, "interface bind failed\n");
		throw new SharkException("interfaceFd bind failed");
	}

	ret = listen(interfaceFd, 5);
	if (ret < 0) {
		sharkLog(SHARK_LOG_ERR, "interfaceFd listen failed\n");
		throw new SharkException("interfaceFd listen failed");
	}

	rcvBuf = malloc(RCV_BUF_SIZE);
	if (rcvBuf == NULL) {
		sharkLog(SHARK_LOG_ERR, "rcv buf malloc failed\n");
		throw new SharkException("rcvBuf malloc failed");
	}
	memset(rcvBuf, 0, RCV_BUF_SIZE);

	threadInit();

	sharkLog(SHARK_LOG_ERR, "CliServer construct successfully\n");
	return;
}

shark::CliServer::~CliServer() {
	if (rcvBuf != NULL) {
		free(rcvBuf);
	}

	if (interfaceFd != 0) {
		close(interfaceFd);
	}

	sharkLog(SHARK_LOG_INFO, "CliServer destruct successfully\n");
	return;
}

std::string shark::CliServer::cliProcess(char *buf) {
	Command cmd;
	std::string retStr;

	nlohmann::json cliJson = nlohmann::json::parse(std::string(buf));

	cmd.type = (COMMAND_TYPE)cliJson["type"].get<int>();
	cmd.cfg.id = cliJson["id"].get<std::string>();
	cmd.cfg.execCmd = cliJson["execCmd"].get<std::string>();
	cmd.cfg.net.addr.str = cliJson["ipAddr"].get<std::string>();
	cmd.cfg.net.bandwidth = cliJson["netBandwidth"].get<std::string>();

	retStr = cPool.cmdProcess(&cmd);

	sharkLog(SHARK_LOG_DEBUG, "cliRet:%s\n",retStr.data());
	return retStr;
}

int shark::CliServer::cliRecv() {
	int rcvLen = 0;
	int ret = 0;
	unsigned int clientAddrLen = 0;
	struct sockaddr_un clientAddr;
	std::string retStr;

	while(1) {
		bzero(&clientAddr, sizeof(clientAddr));
		ret = sepoll.wait(interfaceFd, -1);
		if (ret == -1) {
			sharkLog(SHARK_LOG_DEBUG, "epoll wait failed\n");
			return -1;
		}

		int recvFd = accept(interfaceFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
		if (recvFd < 0) {

			sharkLog(SHARK_LOG_ERR, "accept failed\n");
			throw new SharkException("accept failed");
		}

		memset(rcvBuf, 0, RCV_BUF_SIZE);
		rcvLen = recv(recvFd, rcvBuf, RCV_BUF_SIZE, MSG_CMSG_CLOEXEC);
		if (rcvLen < 0) {
			throw new SharkException("received failed");
		}

		retStr = cliProcess((char *)rcvBuf);

		ret = send(recvFd, retStr.data(), retStr.size(), 0);
		sharkLog(SHARK_LOG_DEBUG, "cliRecvRet:%d,%s\n", ret, retStr.data());
	}

	return 0;
}

int shark::CliServer::retSend(const void *data, int dataSize) {
	int interfaceFd = 0;
	int ret = 0;

	ret = send(interfaceFd, data, dataSize, MSG_NOSIGNAL);
	if (ret != (signed int)dataSize) {
		sharkLog(SHARK_LOG_ERR, "cmd send failed\n");
		throw new SharkException("Json Command send failed");
	}

	close(interfaceFd);
	sharkLog(SHARK_LOG_DEBUG, "cmd sent successfully\n");
	return ret;
}

void* shark::CliServer::threadFunc(void *arg) {
	CliServer *thisPtr = (CliServer *)arg;

	thisPtr->cliRecv();

	return NULL;
}

int shark::CliServer::threadInit() {
	int ret = 0;
	pthread_attr_t threadAttr =  {0};

	ret = pthread_create(&receiveThread, &threadAttr, threadFunc, this);
	if (ret < 0) {
		sharkLog(SHARK_LOG_ERR, "CliServer threadInit failed\n");
		throw new SharkException("rcvThread create failed");
	}

	sharkLog(SHARK_LOG_DEBUG, "CliServer threadInit successfully\n");
	return 0;
}
