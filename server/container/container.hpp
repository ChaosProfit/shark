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
#include "config/containerConfig.hpp"

namespace shark {
	class Container{
	public:
		Container(ContainerConfig &cCfg, SharkConfig &sCfg);
		~Container();

		int checkPoint();
		int restore();

		int pause();
		int resume();

		int start();
		int stop();
		int cmdSend(std::string &execCmd);

		std::string& getId(){
			return cCfg.id;
		};

		int getReadPipe(){
			return manageProcessPipe[0];
		}

		int addProcess(Process *p);
	private:
		int dftCfgInit();
		int initManageProcess();
		Process *manageProcess = NULL;
		int manageProcessPipe[2] = {0};

		std::string id;
		std::string nameSpace;
		ContainerConfig cCfg;
		SharkConfig &sCfg;
		std::list<Process *> processList;

		ContainerNetwork *cNetwork;

		const static int SHORT_ID_LENGTH = 32;
	};
}

#endif /* CONTAINER_CONTAINER_HPP_ */
