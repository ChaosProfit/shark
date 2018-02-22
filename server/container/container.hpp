/*
 * factory.hpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#ifndef CONTAINER_CONTAINER_HPP_
#define CONTAINER_CONTAINER_HPP_


#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <string>
#include <memory>
#include <list>

#include "process/process.hpp"
#include "utils/threadQueue.hpp"
#include "utils/command.hpp"
#include "network/containerNetwork.hpp"
#include "network/networkConfig.hpp"
#include "containerConfig.hpp"

namespace shark {
	class Container{
	public:
		Container(struct ContainerConfig &cfg);
		~Container();

		int checkPoint();
		int restore();

		int pause();
		int resume();

		int start();
		int stop();
		int cmdSend(std::string &execCmd);

		std::string& getId(){
			return cfg.id;
		};

		int getReadPipe(){
			return manageProcessPipe[0];
		}

		int addProcess(Process *p);
	private:
		int initManageProcess();
		Process *manageProcess = NULL;
		int manageProcessPipe[2] = {0};

		std::string id;
		std::string nameSpace;
		struct ContainerConfig cfg;
		std::list<Process *> processList;

		ContainerNetwork * cNetwork;

		const static int SHORT_ID_LENGTH = 32;
	};
}

#endif /* CONTAINER_CONTAINER_HPP_ */
