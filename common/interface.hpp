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

#include "utils/threadQueue.hpp"
#include "utils/sepoll.hpp"
#include "command.hpp"

namespace shark{

	typedef enum{
		INTERFACE_NONE = 0,
		INTERFACE_SYNC,
		INTERFACE_ASYNC
	}INTERFACE_TYPE;

	class Interface{
	public:
		Interface();
		Interface(ThreadQueue<struct Command *> *cmdQueue);
		Interface(ThreadQueue<struct Command *> *cmdQueue, int (*fn)(void *object, void* data), void *object);
		~Interface();

		int dataRecv();
		int dataRecv(int timeout);
		int dataSend(const void *data, int size);
		void *getDataPtr(){
			return rcvBuf;
		};

	private:
		int initCommon();
		int process(char *buf);
		static void *threadFunc(void *);
		int threadInit();

		pthread_t receiveThread = 0;
		ThreadQueue<struct Command *> *cmdQueue;

		INTERFACE_TYPE syncType = INTERFACE_NONE;

		int timeout = 0;
		Sepoll sepoll = Sepoll();
		int interfaceFd = 0;
		const char* SHARK_INTERFACE_PATH = "/run/shark/interface";
		int RCV_BUF_SIZE = 1024;
		void *rcvBuf = NULL;
		struct sockaddr_un addr;

		int (*dataProcess)(void *object, void *data) = NULL;
		void *releObject = NULL;
	};
}

#endif /* SHARKD_INTERFACE_HPP_ */
