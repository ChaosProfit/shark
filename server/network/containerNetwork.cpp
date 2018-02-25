/*
 * containerNetwork.cpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */

#include "containerNetwork.hpp"
#include "globalNetwork.hpp"
#include "utils/misc.hpp"
#include "utils/log.hpp"

int shark::ContainerNetwork::bridgeInit(){
	int ret = 0;
	std::string netNs = "shark" + shortId;

	ret = cmdExecSync("ip netns add %s", netNs.data());
	ret = cmdExecSync("ip link add %s type veth peer name veth%s", ETH0_INTERFACE, netNs.data());
	ret = cmdExecSync("ip link set %s netns %s", ETH0_INTERFACE, netNs.data());
	ret = cmdExecSync("ip netns exec %s ip addr add %s dev %s", netNs.data(), this->cnCfg.addr.str.data(), ETH0_INTERFACE);
	ret = cmdExecSync("ip netns exec %s ip link set up dev %s", netNs.data(), ETH0_INTERFACE);
	ret = cmdExecSync("ip netns exec %s ip link set up dev lo", netNs.data());
	ret = cmdExecSync("ip link set veth%s master %s", netNs.data(), gnCfg.bridge.name.data());

	sharkLog(SHARK_LOG_DEBUG, "Container %s bridge init\n", shortId.data());
	return ret;
}

int shark::ContainerNetwork::bridgeExit(){
	int ret = 0;
	std::string netNs = "shark" + shortId;

	ret = cmdExecSync("ip link set veth%s nomaster", netNs.data());
	ret = cmdExecSync("ip netns exec %s ip link set %s down", netNs.data(), ETH0_INTERFACE);
	ret = cmdExecSync("ip netns del %s", netNs.data());

	sharkLog(SHARK_LOG_DEBUG, "Container %s bridge exit\n", shortId.data());
	return ret;
}

shark::ContainerNetwork::ContainerNetwork(std::string &sId, NetworkConfig &gnCfgArg, ContainerNetworkConfig &cnCfgArg):
shortId(sId), gnCfg(gnCfgArg), cnCfg(cnCfgArg){
	int ret = 0;

	if(gnCfg.enable == false){
		sharkLog(SHARK_LOG_DEBUG, "Container %s Network %d construct finished, Network disabled\n", shortId.data(), gnCfg.type);
		return;
	}

	ret = ipv4AddrPreprocess(cnCfg.addr);
	if(ret < 0){
		sharkLog(SHARK_LOG_DEBUG, "%s preprocess failed\n", cnCfg.addr.str.data());
		return;
	}

	switch(gnCfg.type){
	case NETWORK_BRIDGE:
		bridgeInit();
		break;
	default:
		sharkLog(SHARK_LOG_ERR, "container:%s, Unknown network type:%d\n", shortId.data(), gnCfg.type);
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "Container %s Network %d construct successfully\n", shortId.data(), gnCfg.type);
	return;
}

shark::ContainerNetwork::~ContainerNetwork(){
	if(gnCfg.enable == false){
		sharkLog(SHARK_LOG_DEBUG, "Container %s Network %d destruct finished, Network disabled\n", shortId.data(), gnCfg.type);
		return;
	}

	switch(gnCfg.type){
	case NETWORK_BRIDGE:
		bridgeExit();
		break;
	default:
		sharkLog(SHARK_LOG_ERR, "container:%s, Unknown network type:%d\n", shortId.data(), gnCfg.type);
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "Container %s Network %d, destruct successfully\n", shortId.data(), gnCfg.type);
	return;
}
