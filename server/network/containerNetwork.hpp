/*
 * containerNetwork.hpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */

#ifndef NETWORK_CONTAINERNETWORK_HPP_
#define NETWORK_CONTAINERNETWORK_HPP_

#include "config/globalConfig.hpp"
#include <string>

namespace shark{
	class ContainerNetwork{
	public:
		ContainerNetwork(std::string &shortId, NetworkConfig &nCfg);
		~ContainerNetwork();
	private:
		int bridgeInit();
		int bridgeExit();

		std::string &shortId;
		NetworkConfig &nCfg;
	};
}

#endif /* NETWORK_CONTAINERNETWORK_HPP_ */
