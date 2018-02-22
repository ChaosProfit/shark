/*
 * network.hpp
 *
 *  Created on: 2018年2月19日
 *      Author: luguanglong
 */

#ifndef NETWORK_GLOBALNETWORK_HPP_
#define NETWORK_GLOBALNETWORK_HPP_

#include "networkConfig.hpp"

namespace shark{

#define SHARK_BRIDGE "shark0"
	class GlobalNetwork{
	public:
		GlobalNetwork(NETWORK_TYPE nType);
		~GlobalNetwork();

	private:
		int bridgeInit();
		int bridgeExit();

		NETWORK_TYPE nType;
	};
}


#endif /* NETWORK_GLOBALNETWORK_HPP_ */
