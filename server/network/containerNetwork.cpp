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
	ret = cmdExecSync("ip link add veth0 type veth peer name veth%s", netNs.data());
	ret = cmdExecSync("ip link set veth0 netns %s", netNs.data());
	ret = cmdExecSync("ip netns exec %s ip link set veth0 up", netNs.data());
	ret = cmdExecSync("ip link set veth%s master %s", netNs.data(), SHARK_BRIDGE);

	sharkLog(SHARK_LOG_DEBUG, "Container %s bridge init\n", shortId.data());
	return ret;
}

int shark::ContainerNetwork::bridgeExit(){
	int ret = 0;
	std::string netNs = "shark" + shortId;

	ret = cmdExecSync("ip link set veth%s nomaster", netNs.data());
	ret = cmdExecSync("ip netns exec %s ip link set veth0 down", netNs.data());
	ret = cmdExecSync("ip netns del %s", netNs.data());

	sharkLog(SHARK_LOG_DEBUG, "Container %s bridge exit\n", shortId.data());
	return ret;
}

shark::ContainerNetwork::ContainerNetwork(std::string &sId, NETWORK_TYPE nType):
shortId(sId), nType(nType){

	switch(nType){
	case NETWORK_BRIDGE:
		bridgeInit();
		break;
	default:
		sharkLog(SHARK_LOG_ERR, "container:%s, Unknown network type:%d\n", shortId.data(), nType);
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "Container %s Network %d construct successfully\n", shortId.data(), nType);
	return;
}

shark::ContainerNetwork::~ContainerNetwork(){
	switch(nType){
	case NETWORK_BRIDGE:
		bridgeExit();
		break;
	default:
		sharkLog(SHARK_LOG_ERR, "container:%s, Unknown network type:%d\n", shortId.data(), nType);
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "Container %s Network %d, destruct successfully\n", shortId.data(), nType);
	return;
}
