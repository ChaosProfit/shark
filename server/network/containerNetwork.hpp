/*
 * containerNetwork.hpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */

#ifndef NETWORK_CONTAINERNETWORK_HPP_
#define NETWORK_CONTAINERNETWORK_HPP_

#include "config/containerConfig.hpp"
#include <string>

namespace shark{
	class ContainerNetwork{
	public:
		ContainerNetwork(std::string &sId, NetworkConfig &gnCfgArg, ContainerNetworkConfig &cnCfgArg);
		~ContainerNetwork();
	private:
		int bridgeInit();
		int bridgeExit();

		std::string &shortId;
		NetworkConfig gnCfg;
		ContainerNetworkConfig cnCfg;

		const char ETH0_INTERFACE[8] = "eth0";
	};
}

#endif /* NETWORK_CONTAINERNETWORK_HPP_ */
