/*
 * containerNetwork.hpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */

#ifndef NETWORK_CONTAINERNETWORK_HPP_
#define NETWORK_CONTAINERNETWORK_HPP_

#include <string>

#include "config/containerConfig.hpp"

namespace shark {
class ContainerNetwork {
 public:
	ContainerNetwork(std::string &sId, NetworkConfig &gnCfgArg, ContainerNetworkConfig &cnCfgArg);
	~ContainerNetwork();

 private:
	int commonInit();
	int commonExit();
	int bridgeInit();
	int bridgeExit();
	int bandwidthSet();
	int bandwidthClear();

	std::string &shortId;
	NetworkConfig gnCfg;
	ContainerNetworkConfig cnCfg;

	const char ETH0_INTERFACE[8] = "eth0";
};
}  // namespace shark

#endif /* NETWORK_CONTAINERNETWORK_HPP_ */
