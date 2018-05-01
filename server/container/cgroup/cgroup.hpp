/*
 * cgroup.hpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#ifndef LIBCONTAINER_CGROUP_CGROUP_HPP_
#define LIBCONTAINER_CGROUP_CGROUP_HPP_

#include <stddef.h>

#include <vector>
#include <string>

/*
	Cgroup class defines all the cgroup operations.
*/
namespace shark {
typedef struct {
	std::string id;
}CgroupConfig;

class Cgroup {
 public:
	Cgroup();
	~Cgroup();

	/*Add a leaf, correspond to a container*/
	int addLeaf(std::string &id);
	/*Add a leaf, correspond to a container*/
	int delLeaf(std::string id);
	/*Add a task, correspond to a process in a container*/
	int addTask(std::string id, int pid);

	/*select which cpu to run, default is all*/
	int setCpuSelect(std::string id, int cpuNum);
	/*set how much cpu could be used, unit is 1/100000*/
	int setCpuQuota(std::string id, int quota);
	/*set how many memory could be allocated, unit is bytes*/
	int setMemQuota(std::string id, int quota);

 private:
	int setIntValue(const char *path, int value, bool append);
	int setStrValue(const char *path, const char *value, bool append);

	int getValue();


	std::vector<std::string> subModules =  {"devices", "perf_event", "memory", "cpuset",
										   "blkio", "cpu,cpuacct", "net_cls,net_prio",
										   "freezer", "pids", "systemd"};

	std::string cgroupPath = "/sys/fs/cgroup/";
};
}  // namespace shark

#endif /* LIBCONTAINER_CGROUP_CGROUP_HPP_ */
