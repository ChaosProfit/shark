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

#include "interface.hpp"
#include "utils/exceptions.hpp"
#include "utils/log.hpp"

int shark::Interface::initCommon(){
	int ret = 0;

	unlink(SHARK_INTERFACE_PATH);
	interfaceFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(interfaceFd < 0){
		sharkLog(SHARK_LOG_ERR, "interfaceFd socker failed\n");
		throw SharkException("interfaceFd socket failed");
	}

	sepoll.addPort(interfaceFd);

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SHARK_INTERFACE_PATH, sizeof(addr.sun_path) - 1);
	ret = bind(interfaceFd, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0){
		sharkLog(SHARK_LOG_ERR, "interface bind failed\n");
		throw SharkException("interfaceFd bind failed");
	}

	ret = listen(interfaceFd, 5);
	if(ret < 0){
		sharkLog(SHARK_LOG_ERR, "interfaceFd listen failed\n");
		throw SharkException("interfaceFd listen failed");
	}

	rcvBuf = malloc(RCV_BUF_SIZE);
	if(rcvBuf == NULL){
		sharkLog(SHARK_LOG_ERR, "rcv buf malloc failed\n");
		throw SharkException("rcvBuf malloc failed");
	}
	memset(rcvBuf, 0, RCV_BUF_SIZE);

	sharkLog(SHARK_LOG_DEBUG, "11111, interfaceFd:%d\n", interfaceFd);
	sharkLog(SHARK_LOG_DEBUG, "initCommon successfully\n");
	return 0;
}

shark::Interface::Interface(ThreadQueue<struct Command *> *cmdQueue, int (*fn)(void *object, void *data), void *object) :
cmdQueue(cmdQueue), dataProcess(fn), releObject(object){

	initCommon();

	threadInit();

	sharkLog(SHARK_LOG_ERR, "Interface construct successfully\n");
	return;
}

shark::Interface::Interface(ThreadQueue<struct Command *> *cmdQueue) :
cmdQueue(cmdQueue){

	initCommon();

	sharkLog(SHARK_LOG_DEBUG, "Interface construct successfully\n");
	return;
}

shark::Interface::Interface(){
	initCommon();

	return;
}

shark::Interface::~Interface(){
	if (rcvBuf != NULL){
		free(rcvBuf);
	}

	if(interfaceFd != 0){
		close(interfaceFd);
	}

	sharkLog(SHARK_LOG_INFO, "Interface destruct successfully\n");
	return;
}

int shark::Interface::dataRecv(int timeout){
	int rcvLen = 0;
	int ret = 0;
	unsigned int clientAddrLen = 0;
	struct sockaddr_un clientAddr;

	bzero(&clientAddr, sizeof(clientAddr));
	ret = sepoll.wait(interfaceFd, timeout);
	if(ret == 0){
		sharkLog(SHARK_LOG_DEBUG, "epoll wait timeout\n");
		return 0;
	}

	if(ret == -1){
		sharkLog(SHARK_LOG_ERR, "epoll wait failed\n");
		return -1;
	}

	int recvFd = accept(interfaceFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
	if(recvFd < 0){

		sharkLog(SHARK_LOG_ERR, "accept failed\n");
		throw SharkException("accept failed");
	}

	memset(rcvBuf, 0, rcvLen);
	rcvLen = recv(recvFd, rcvBuf, RCV_BUF_SIZE, MSG_CMSG_CLOEXEC);
	if(rcvLen < 0){
		sharkLog(SHARK_LOG_ERR, "recv failed,errno:%d\n", errno);
		throw SharkException("received failed");
	}

	sharkLog(SHARK_LOG_DEBUG, "rcvLen:%d\n",rcvLen);
	return rcvLen;
}


int shark::Interface::dataRecv(){
	int rcvLen = 0;
	int ret = 0;
	unsigned int clientAddrLen = 0;
	struct sockaddr_un clientAddr;

	while(1){
		sharkLog(SHARK_LOG_DEBUG, "11111\n");
		bzero(&clientAddr, sizeof(clientAddr));
		sharkLog(SHARK_LOG_DEBUG, "11111\n");
		ret = sepoll.wait(interfaceFd, -1);
		if(ret == -1){
			sharkLog(SHARK_LOG_DEBUG, "epoll wait failed\n");
			return -1;
		}
		sharkLog(SHARK_LOG_DEBUG, "11111\n");
		int recvFd = accept(interfaceFd, (struct sockaddr *)&clientAddr, &clientAddrLen);
		if(recvFd < 0){

			sharkLog(SHARK_LOG_ERR, "accept failed\n");
			throw SharkException("accept failed");
		}
		sharkLog(SHARK_LOG_DEBUG, "11111\n");
		memset(rcvBuf, 0, rcvLen);
		sharkLog(SHARK_LOG_DEBUG, "11111\n");
		rcvLen = recv(recvFd, rcvBuf, RCV_BUF_SIZE, MSG_CMSG_CLOEXEC);
		if(rcvLen < 0){
			sharkLog(SHARK_LOG_ERR, "recv failed,errno:%d\n", errno);
			throw SharkException("received failed");
		}
		sharkLog(SHARK_LOG_DEBUG, "11111\n");
		if(dataProcess != NULL){
			sharkLog(SHARK_LOG_DEBUG, "11111\n");
			dataProcess(releObject, rcvBuf);
			sharkLog(SHARK_LOG_DEBUG, "11111\n");
		}
		sharkLog(SHARK_LOG_DEBUG, "11111\n");
		sharkLog(SHARK_LOG_DEBUG, "rcvLen:%d\n",rcvLen);
	}

	return 0;
}

int shark::Interface::dataSend(const void *data, int dataSize){
	int interfaceFd = 0;
	int ret = 0;
	struct sockaddr_un addr;

	interfaceFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(interfaceFd < 0){
		sharkLog(SHARK_LOG_ERR, "socket create failed\n");
		throw SharkException("interfaceFd socket failed");
	}

	bzero(&addr, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SHARK_INTERFACE_PATH, sizeof(addr.sun_path) - 1);

	ret = connect(interfaceFd, (struct sockaddr *)&addr, sizeof(addr));
	if(ret < 0){
		sharkLog(SHARK_LOG_ERR, "socket connect failed, errno:%d\n", errno);
		throw SharkException("connect failed");
	}

	ret = send(interfaceFd, data, dataSize, MSG_NOSIGNAL);
	if(ret != (signed int)dataSize){
		sharkLog(SHARK_LOG_ERR, "cmd send failed\n");
		throw SharkException("Json Command send failed");
	}

	close(interfaceFd);

	sharkLog(SHARK_LOG_DEBUG, "cmd sent successfully\n");
	return 0;
}

void* shark::Interface::threadFunc(void *arg){
	Interface *thisPtr = (Interface *)arg;

	thisPtr->dataRecv();

	return NULL;
}

int shark::Interface::threadInit(){
	int ret = 0;
	pthread_attr_t threadAttr = {0};

	ret = pthread_create(&receiveThread, &threadAttr, threadFunc, this);
	if(ret < 0){
		sharkLog(SHARK_LOG_ERR, "Interface threadInit failed\n");
		throw SharkException("rcvThread create failed");
	}

	sharkLog(SHARK_LOG_DEBUG, "Interface threadInit successfully\n");
	return 0;
}
