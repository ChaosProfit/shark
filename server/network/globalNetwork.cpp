/*
 * network.cpp
 *
 *  Created on: 2018年2月19日
 *      Author: luguanglong
 */

#include "globalNetwork.hpp"
#include "utils/misc.hpp"
#include "utils/log.hpp"

int shark::GlobalNetwork::bridgeInit(){
	int ret = 0;

	ret = cmdExecSync("ip link add name %s type bridge", SHARK_BRIDGE);
	ret = cmdExecSync("ip link set %s up", SHARK_BRIDGE);

	sharkLog(SHARK_LOG_DEBUG, "Bridge init successfully\n");
	return ret;
}

int shark::GlobalNetwork::bridgeExit(){
	int ret = 0;

	ret = cmdExecSync("ip link set %s down", SHARK_BRIDGE);
	ret = cmdExecSync("ip link del name %s type bridge", SHARK_BRIDGE);

	sharkLog(SHARK_LOG_DEBUG, "Bridge exit successfully\n");
	return ret;
}

shark::GlobalNetwork::GlobalNetwork(NETWORK_TYPE nType):
nType(nType){
	int ret = 0;

	switch(nType){
	case NETWORK_BRIDGE:
		ret = bridgeInit();
		break;
	default:
		sharkLog(SHARK_LOG_ERR, "Wrong network type\n");
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "Network construct successfully, ret:%d\n", ret);
}

shark::GlobalNetwork::~GlobalNetwork(){
	int ret = 0;

	switch(nType){
	case NETWORK_BRIDGE:
		ret = bridgeExit();
		break;
	default:
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "Network destruct successfully, ret:%d\n", ret);
}
