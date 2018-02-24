/*
 * network.hpp
 *
 *  Created on: 2018年2月19日
 *      Author: luguanglong
 */

#ifndef NETWORK_GLOBALNETWORK_HPP_
#define NETWORK_GLOBALNETWORK_HPP_

#include "config/globalConfig.hpp"

namespace shark{

#define SHARK_BRIDGE "shark0"
	class GlobalNetwork{
	public:
		GlobalNetwork(NetworkConfig &cfg);
		~GlobalNetwork();

	private:
		int flagGet(char *flagPath, int &value);
		int flagSet(char *flagPath, int value);
		int bridgeIptablesInit();
		int bridgeIptablesExit();
		int bridgeInit();
		int bridgeExit();

		NetworkConfig &nCfg;

		const char sharkChain[32] = "SHARK";
		int originalIpv4ForwardFlag = 0;
	};
}


#endif /* NETWORK_GLOBALNETWORK_HPP_ */
