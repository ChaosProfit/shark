/*
 * interface.hpp
 *
 *  Created on: Jan 24, 2018
 *      Author: luguanglong
 */

#ifndef SHARKD_INTERFACE_HPP_
#define SHARKD_INTERFACE_HPP_

#include <sys/un.h>
#include <pthread.h>

#include <string>
#include <deque>
#include <memory>

#include "container/containerPool.hpp"
#include "utils/threadQueue.hpp"
#include "utils/sepoll.hpp"
#include "utils/command.hpp"

namespace shark {
enum class INTERFACE_TYPE {
	INTERFACE_NONE = 0,
	INTERFACE_SYNC,
	INTERFACE_ASYNC
};

class CliServer {
 public:
	explicit CliServer(ContainerPool &cPool);
	~CliServer();

	int cliRecv();
	int retSend(const void *data, int size);
	void *getDataPtr() {
		return rcvBuf;
	}

 private:
	std::string cliProcess(char *buf);
	int initCommon();
	int process(char *buf);
	static void *threadFunc(void *);
	int threadInit();

	pthread_t receiveThread = 0;

	INTERFACE_TYPE syncType = INTERFACE_TYPE::INTERFACE_NONE;

	ContainerPool &cPool;
	int timeout = 0;
	Sepoll sepoll = Sepoll();
	int interfaceFd = 0;
	const char* SHARK_INTERFACE_PATH = "/run/shark/cli";
	int RCV_BUF_SIZE = 1024;
	void *rcvBuf = nullptr;
	struct sockaddr_un addr;
};
}  // namespace shark

#endif /* SHARKD_INTERFACE_HPP_ */
