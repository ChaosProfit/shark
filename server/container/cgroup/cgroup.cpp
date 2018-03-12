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
#include <stdio.h>

#include <string>
#include <vector>
#include <fstream>

#include "cgroup.hpp"
#include "utils/log.hpp"

int shark::Cgroup::setIntValue(const char *path, int value, bool append){
	char tmp[36] = {0};

	snprintf(tmp, 36, "%d", value);

	return setStrValue(path, tmp, append);
}

int shark::Cgroup::setStrValue(const char *path, const char *value, bool append){
	int ret = 0;
	int flags = O_WRONLY;

	if(append == true){
		flags |= O_APPEND;
	}

	int fd = open(path, flags);
	if(fd < 0){
		sharkLog(SHARK_LOG_ERR, "open %s failed\n", path);
		return -1;
	}

	ret = write(fd, value, strlen(value));
	if(ret < 0){
		close(fd);
		sharkLog(SHARK_LOG_ERR, "write %s to %s failed\n", value, path);
		return -1;
	}

	close(fd);

	sharkLog(SHARK_LOG_DEBUG, "set %s to %s successfully\n", value, path);
	return 0;
}

int shark::Cgroup::setCpuSelect(std::string id, int cpuNum){
	std::string path = cgroupPath + "cpuset/shark/" + id + "/cpuset.cpus";

	int ret = setIntValue(path.data(), cpuNum, false);

	sharkLog(SHARK_LOG_DEBUG, "container %s select cpu %d, ret:%d\n",
			id.data(), cpuNum, ret);
	return 0;
}

int shark::Cgroup::setCpuQuota(std::string id, int cpuQuota){
	std::string path = cgroupPath + "cpu,cpuacct/" + "/cpu.cfs_quota_us";

	int ret = setIntValue(path.data(), cpuQuota, false);

	sharkLog(SHARK_LOG_DEBUG, "container %s set cpu %d, ret:%d\n",
								   id.data(), cpuQuota, ret);
	return 0;
}

int shark::Cgroup::setMemQuota(std::string id, int memQuota){
	std::string path = cgroupPath + "memory/" + "/memory.limit_in_bytes";

	int ret = setIntValue(path.data(), memQuota, false);

	sharkLog(SHARK_LOG_DEBUG, "container %s set mem quota %d, ret:%d\n",
								   id.data(), memQuota, ret);
	return 0;
}

int shark::Cgroup::addTask(std::string id, int pid){
	int ret = 0;
	std::string tasksPath;
	char pidBuf[36] = {0};

	snprintf(pidBuf, 36, "%d\n", pid);

	for(auto item = subModules.begin(); item != subModules.end(); item++){
		tasksPath = cgroupPath + *item + "/" + id + "/" + "tasks";

		ret = setStrValue(tasksPath.data(), pidBuf, true);
		if(ret < 0){
			sharkLog(SHARK_LOG_DEBUG, "update %s to %s failed\n", pidBuf, tasksPath.data());
		}
	}

	sharkLog(SHARK_LOG_DEBUG, "shark %s add %d cgroup successfully\n", id.data(), pid);
	return 0;
}

int shark::Cgroup::addLeaf(std::string &id){
	int ret = 0;
	std::string dirPath;

	for(auto item = subModules.begin(); item != subModules.end(); item++){
		dirPath = cgroupPath + *item + "/" + id;
		ret = mkdir(dirPath.data(), 0777);
		if(ret < 0){
			sharkLog(SHARK_LOG_ERR, "makedir %s failed, errno:%d\n", dirPath.data(), errno);
		}
	}

	sharkLog(SHARK_LOG_DEBUG, "add %s leaf successfully\n", id.data());
	return 0;
}

int shark::Cgroup::delLeaf(std::string id){
	int ret = 0;
	std::string dirPath;

	for(auto item = subModules.begin(); item != subModules.end(); item++){
		dirPath = cgroupPath + *item + "/" + id;
		ret = rmdir(dirPath.data());
		if(ret < 0){
			sharkLog(SHARK_LOG_ERR, "makedir %s failed, errno:%d\n", dirPath.data(), errno);
		}
	}

	sharkLog(SHARK_LOG_DEBUG, "del %s leaf successfully\n", id.data());
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
