/*
 * containerConfig.hpp
 *
 *  Created on: 2018年2月21日
 *      Author: luguanglong
 */

#ifndef CONTAINER_CONTAINERCONFIG_HPP_
#define CONTAINER_CONTAINERCONFIG_HPP_

#include <string>
#include <list>

#include "globalConfig.hpp"

namespace shark {

	typedef struct {
		Ipv4Addr addr;
		std::string bandwidth;
	} ContainerNetworkConfig;

	typedef struct {
		int cpuSelect;	/*The number of which cpu to select. If -1, there is no limitation.*/
		int cpuUsage;	/* The percent ratio of cpu usage:0~100. If -1, there is no limitation.*/
		int memUsage;	/*The unit is kbyte. If -1, there is no limitation.*/
	} ContainerCgroupConfig;

	typedef struct {
		std::string id;
		std::string execCmd;
		std::string nameSpace;
		std::string rootfsPath;
		ContainerNetworkConfig net;
		ContainerCgroupConfig cgroup;
	} ContainerConfig;
}

#endif /* CONTAINER_CONTAINERCONFIG_HPP_ */
