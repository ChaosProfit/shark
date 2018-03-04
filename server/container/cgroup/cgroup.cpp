/*
 * cgroup.cpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#include <string>
#include <vector>

#include "cgroup.hpp"
#include "utils/log.hpp"

int shark::Cgroup::addTask(std::string id, int pid){
	int ret = 0;
	int taskFd = 0;
	std::string tasksPath;
	char pidBuf[36] = {0};

	snprintf(pidBuf, 36, "%d\n", pid);
	for(auto item = subModules.begin(); item != subModules.end(); item++){
		tasksPath = cgroupPath + *item + "/" + id + "/" + "tasks";
		taskFd = open(tasksPath.data(), O_WRONLY|O_APPEND);
		if(taskFd < 0){
			sharkLog(SHARK_LOG_ERR, "open %s failed\n", tasksPath.data());
			continue;
		}

		ret = write(taskFd, pidBuf, strlen(pidBuf));
		if(ret != strlen(pidBuf)){
			sharkLog(SHARK_LOG_ERR, "write %s to %s failed\n", pidBuf, tasksPath.data());
		}

		close(taskFd);
	}

	sharkLog(SHARK_LOG_DEBUG, "shark %s add %d cgroup successfully\n", id.data(), pid);
	return 0;
}

int shark::Cgroup::addLeaf(CgroupConfig &cfg){
	int ret = 0;
	std::string dirPath;

	for(auto item = subModules.begin(); item != subModules.end(); item++){
		dirPath = cgroupPath + *item + "/" + cfg.id;
		ret = mkdir(dirPath.data(), 0777);
		if(ret < 0){
			sharkLog(SHARK_LOG_ERR, "makedir %s failed, errno:%d\n", dirPath.data(), errno);
		}
	}

	sharkLog(SHARK_LOG_DEBUG, "add %s leaf successfully\n", cfg.id.data());
	return 0;
}

int shark::Cgroup::delLeaf(CgroupConfig &cfg){
	int ret = 0;
	std::string dirPath;

	for(auto item = subModules.begin(); item != subModules.end(); item++){
		dirPath = cgroupPath + *item + "/" + cfg.id;
		ret = rmdir(dirPath.data());
		if(ret < 0){
			sharkLog(SHARK_LOG_ERR, "makedir %s failed, errno:%d\n", dirPath.data(), errno);
		}
	}

	sharkLog(SHARK_LOG_DEBUG, "del %s leaf successfully\n", cfg.id.data());
	return 0;
}

shark::Cgroup::Cgroup(){
	int ret = 0;

	std::string dirPath;
	for(auto item = subModules.begin(); item != subModules.end(); item++){
		dirPath = cgroupPath + *item;
		dirPath.append("/shark");
		ret = mkdir(dirPath.data(), 0777);
		if(ret < 0){
			sharkLog(SHARK_LOG_ERR, "makedir %s failed, errno:%d\n", dirPath.data(), errno);
		}
	}

	sharkLog(SHARK_LOG_DEBUG, "Cgroup construct successfully\n");
	return;
};

shark::Cgroup::~Cgroup(){
	int ret = 0;
	std::string dirPath;
	for(auto item = subModules.begin(); item != subModules.end(); item++){
		dirPath = cgroupPath + *item;
		dirPath.append("/shark");
		ret = rmdir(dirPath.data());
		if(ret < 0){
			sharkLog(SHARK_LOG_ERR, "makedir %s failed, errno:%d\n", dirPath.data(), errno);
		}
	}

	sharkLog(SHARK_LOG_DEBUG, "Cgroup destruct successfully\n");
	return;
};
