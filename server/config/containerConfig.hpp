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

namespace shark{

	typedef struct{
		Ipv4Addr addr;
	} ContainerNetworkConfig;

	typedef struct{
		std::string id;
		std::string execCmd;
		std::string nameSpace;
		std::string rootfsPath;
		ContainerNetworkConfig net;
	} ContainerConfig;
}

#endif /* CONTAINER_CONTAINERCONFIG_HPP_ */
