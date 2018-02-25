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
		ContainerNetwork(std::string &sId, NetworkConfig &gCfg, ContainerNetworkConfig &cCfg);
		~ContainerNetwork();
	private:
		int bridgeInit();
		int bridgeExit();

		std::string &shortId;
		NetworkConfig &gnCfg;
		ContainerNetworkConfig &cnCfg;
	};
}

#endif /* NETWORK_CONTAINERNETWORK_HPP_ */
