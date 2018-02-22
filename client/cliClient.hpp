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

namespace shark{

	typedef enum{
		INTERFACE_NONE = 0,
		INTERFACE_SYNC,
		INTERFACE_ASYNC
	}INTERFACE_TYPE;

	class CliClient{
	public:
		CliClient();
		~CliClient();

		void* dataSend(const void *data, int size);

	private:

		int process(char *buf);

		pthread_t receiveThread = 0;

		INTERFACE_TYPE syncType = INTERFACE_NONE;

		int timeout = 0;
		const char* SHARK_INTERFACE_PATH = "/run/shark/cli";
		int RCV_BUF_SIZE = 1024;
		void *rcvBuf = NULL;
		struct sockaddr_un addr;

		int (*dataProcess)(void *object, void *data) = NULL;
		void *releObject = NULL;
	};
}

#endif /* SHARKD_INTERFACE_HPP_ */
