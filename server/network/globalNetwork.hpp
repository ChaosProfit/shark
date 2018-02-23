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
		int bridgeInit();
		int bridgeExit();

		NetworkConfig &nCfg;
	};
}


#endif /* NETWORK_GLOBALNETWORK_HPP_ */
