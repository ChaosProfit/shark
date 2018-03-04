/*
 * cgroup.hpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#ifndef LIBCONTAINER_CGROUP_CGROUP_HPP_
#define LIBCONTAINER_CGROUP_CGROUP_HPP_

#include <stddef.h>

namespace shark{

	typedef struct{
		std::string id;
	}CgroupConfig;

	class Cgroup{
	public:
		Cgroup();
		~Cgroup();

		int addLeaf(CgroupConfig &cfg);
		int delLeaf(CgroupConfig &cfg);
		int addTask(std::string id, int pid);
	private:
	};

	std::vector<std::string> subModules = {"devices", "perf_event", "memory", "cpuset",
										   "blkio", "cpu,cpuacct", "net_cls,net_prio",
										   "freezer", "pids", "systemd"};

	std::string cgroupPath = "/sys/fs/cgroup/";

}



#endif /* LIBCONTAINER_CGROUP_CGROUP_HPP_ */
