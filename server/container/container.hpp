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
#include "cgroup/cgroup.hpp"

/*
	Container class defines a container.
*/
namespace shark  {

	class Container {
	public:
		Container(ContainerConfig &cCfg, SharkConfig &sCfg, Cgroup &cgrp);
		~Container();

		/*pause a container*/
		int pause();
		/*resume a container*/
		int resume();

		/*start a container*/
		int start();
		/*stop a container*/
		int stop();
		/*send a new command to the container and exec*/
		int cmdSend(std::string &execCmd);

		/*get the id of the container*/
		std::string& getId() {
			return cCfg.id;
		};

		/*get the read end of the manageProcessPipe*/
		int getReadPipe() const {
			return manageProcessPipe[0];
		}
		/*add a process to the container*/
		int addProcess(Process *p);

	private:
		int idInit();
		int dftCfgInit();
		int initManageProcess();
		Process *manageProcess = NULL;
		int manageProcessPipe[2] =  {0};

		std::string id;
		std::string nameSpace;
		ContainerConfig cCfg;
		SharkConfig &sCfg;
		std::list<Process *> processList;

		ContainerNetwork *cNetwork;

		Cgroup &cgroup;

		const static int SHORT_ID_LENGTH = 32;
	};
}

#endif /* CONTAINER_CONTAINER_HPP_ */
