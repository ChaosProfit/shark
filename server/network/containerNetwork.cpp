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

int shark::ContainerNetwork::commonInit() {
	int ret = 0;
	std::string netNs = "shark" + shortId;

	ret = cmdExecSync("ip netns add %s", netNs.data());
	ret = cmdExecSync("ip link add %s type veth peer name veth%s", ETH0_INTERFACE, netNs.data());
	ret = cmdExecSync("ip link set %s netns %s", ETH0_INTERFACE, netNs.data());
	ret = cmdExecSync("ip netns exec %s ip addr add %s dev %s", \
					  netNs.data(), this->cnCfg.addr.str.data(), ETH0_INTERFACE);
	ret = cmdExecSync("ip netns exec %s ip link set up dev %s", netNs.data(), ETH0_INTERFACE);
	ret = cmdExecSync("ip netns exec %s ip link set up dev lo", netNs.data());
	ret = cmdExecSync("ip netns exec %s ip route add default via %u.%u.%u.%u dev %s", netNs.data(),
						gnCfg.bridge.addr.value.array[0], gnCfg.bridge.addr.value.array[1],
						gnCfg.bridge.addr.value.array[2], gnCfg.bridge.addr.value.array[3],
						ETH0_INTERFACE);
	ret = cmdExecSync("ip link set up dev veth%s", netNs.data());

	ret = bandwidthSet();

	sharkLog(SHARK_LOG_DEBUG, "Container %s common init\n", shortId.data());
	return ret;
}

int shark::ContainerNetwork::commonExit() {
	int ret = 0;
	std::string netNs = "shark" + shortId;

	ret = cmdExecSync("ip netns del %s", netNs.data());

	ret = bandwidthClear();

	sharkLog(SHARK_LOG_DEBUG, "Container %s common exit\n", shortId.data());
	return ret;
}

int shark::ContainerNetwork::bandwidthSet() {
	int ret = 0;
	std::string netNs = "shark" + shortId;

	if (netNs.size() == 0) {
		sharkLog(SHARK_LOG_DEBUG, "Container %s bandwidth not enabled\n", shortId.data());
		return 0;
	}

	ret = cmdExecSync("tc qdisc add dev veth%s root handle 1: htb default 17", netNs.data());
	ret = cmdExecSync("tc class add dev veth%s parent 1: classid 1:17 htb rate %s ceil %s", \
					  netNs.data(), cnCfg.bandwidth.data(), cnCfg.bandwidth.data());

	sharkLog(SHARK_LOG_DEBUG, \
			"Container %s bandwidthSet:%s finished\n", \
			shortId.data(), cnCfg.bandwidth.data());
	return ret;
}

int shark::ContainerNetwork::bandwidthClear() {
	int ret = 0;
	std::string netNs = "shark" + shortId;

	if (netNs.size() == 0) {
		sharkLog(SHARK_LOG_DEBUG, "Container %s bandwidth not enabled\n", shortId.data());
		return 0;
	}

	ret = cmdExecSync("tc class del dev veth%s parent 1: classid 1:17 htb rate %s ceil %s", \
					  netNs.data(), cnCfg.bandwidth.data(), cnCfg.bandwidth.data());
	ret = cmdExecSync("tc qdisc del dev veth%s root handle 1: htb default 17", netNs.data());

	sharkLog(SHARK_LOG_DEBUG, "Container %s bandwidthClear finished\n", shortId.data());
	return ret;
}

int shark::ContainerNetwork::bridgeInit() {
	int ret = 0;
	std::string netNs = "shark" + shortId;

	ret = cmdExecSync("ip link set veth%s master %s", netNs.data(), gnCfg.bridge.name.data());

	sharkLog(SHARK_LOG_DEBUG, "Container %s bridge init\n", shortId.data());
	return ret;
}

int shark::ContainerNetwork::bridgeExit() {
	sharkLog(SHARK_LOG_DEBUG, "Container %s bridge exit\n", shortId.data());
	return 0;
}

shark::ContainerNetwork::ContainerNetwork(std::string &sId, NetworkConfig &gnCfgArg, \
										  ContainerNetworkConfig &cnCfgArg):
shortId(sId), gnCfg(gnCfgArg), cnCfg(cnCfgArg) {
	int ret = 0;

	if (gnCfg.enable == false) {
		sharkLog(SHARK_LOG_DEBUG, "Container %s Network %d construct finished, Network disabled\n", shortId.data(), gnCfg.type);
		return;
	}

	ret = ipv4AddrPreprocess(cnCfg.addr);
	if (ret < 0) {
		sharkLog(SHARK_LOG_DEBUG, "%s preprocess failed\n", cnCfg.addr.str.data());
		return;
	}

	ret = commonInit();

	switch(gnCfg.type) {
	case NETWORK_BRIDGE:
		ret = bridgeInit();
		break;
	default:
		sharkLog(SHARK_LOG_ERR, "container:%s, Unknown network type:%d\n", shortId.data(), gnCfg.type);
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "Container %s Network %d construct successfully\n", shortId.data(), gnCfg.type);
	return;
}

shark::ContainerNetwork::~ContainerNetwork() {
	int ret = 0;

	if (gnCfg.enable == false) {
		sharkLog(SHARK_LOG_DEBUG, \
				"Container %s Network %d destruct finished, Network disabled\n", \
				shortId.data(), gnCfg.type);
		return;
	}

	switch(gnCfg.type) {
	case NETWORK_BRIDGE:
		bridgeExit();
		break;
	default:
		sharkLog(SHARK_LOG_ERR, "container:%s, Unknown network type:%d\n", shortId.data(), gnCfg.type);
		break;
	}

	ret = commonExit();
	sharkLog(SHARK_LOG_DEBUG, \
			"Container %s Network %d, destruct successfully, ret:%d\n", \
			shortId.data(), gnCfg.type, ret);
	return;
}
