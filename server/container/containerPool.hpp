/*
 * containerPool.hpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#ifndef CONTAINER_CONTAINERPOOL_HPP_
#define CONTAINER_CONTAINERPOOL_HPP_

#include <pthread.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <memory>
#include <list>

#include "container.hpp"
#include "process/process.hpp"
#include "utils/threadQueue.hpp"
#include "utils/command.hpp"
#include "container/containerConfig.hpp"
#include "config/globalConfig.hpp"

namespace shark {
	class ContainerPool{
	public:
		std::string cmdProcess(Command *cmd);

		ContainerPool(GlobalConfig &gCfg);
		~ContainerPool();
	private:
		std::string listCmdProcess();

		std::string retToJson(int ret);
		std::string retToJson(std::string data);
		static void *cmdProcessFunc(void *pool);
		int execContainer(struct Command &cmd);
		int createContainer(struct Command &cfg);
		int delContainer(struct Command &cfg);
		std::list<Container *>& listContainer();
		Container *getContainer(std::string &id);

	private:
		std::list<Container *> clist;
		pthread_t containerThread = 0;
		GlobalConfig &gConfig;

	};
}

#endif /* CONTAINER_CONTAINERPOOL_HPP_ */