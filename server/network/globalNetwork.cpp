/*
 * network.cpp
 *
 *  Created on: 2018年2月19日
 *      Author: luguanglong
 */

#include <string.h>

#include <iostream>
#include <fstream>

#include "globalNetwork.hpp"
#include "utils/misc.hpp"
#include "utils/log.hpp"

int shark::GlobalNetwork::flagGet(char *flagPath, int &value) {
	int ret = 0;
	std::ifstream flag_path;
	char buf[36] =  {0};

	flag_path.open(flagPath);
	flag_path.read(buf, sizeof(buf));
	flag_path.close();
	value = std::atoi(buf);

	sharkLog(SHARK_LOG_DEBUG, "get flag %d from %s\n", value, flagPath);
	return ret;
}

int shark::GlobalNetwork::flagSet(char *flagPath, int value) {
	int ret = 0;
	std::ofstream flag_path;

	char buf[36] =  {0};
	snprintf(buf, sizeof(buf), "%d", value);
	flag_path.open(flagPath);
	flag_path.write(buf, strlen(buf));
	flag_path.close();

	sharkLog(SHARK_LOG_DEBUG, "set flag %d to %s\n", value, flagPath);
	return ret;
}

int shark::GlobalNetwork::bridgeIptablesInit() {
	int ret = 0;

	flagGet("/proc/sys/net/ipv4/ip_forward", originalIpv4ForwardFlag);
	flagSet("/proc/sys/net/ipv4/ip_forward", 1);

	ret = cmdExecSync("iptables -t nat -N %s", sharkChain);
	ret = cmdExecSync("iptables -t nat -A PREROUTING -m addrtype --dst-type LOCAL -j %s", sharkChain);
	ret = cmdExecSync("iptables -t nat -A OUTPUT -m addrtype --dst-type LOCAL -j %s", sharkChain);
	ret = cmdExecSync("iptables -t nat -A POSTROUTING -s %u.%u.%u.%u/%u ! -o %s -j MASQUERADE",
						nCfg.bridge.addr.value.array[0], nCfg.bridge.addr.value.array[1],
						nCfg.bridge.addr.value.array[2], nCfg.bridge.addr.value.array[3],
						nCfg.bridge.addr.mask, nCfg.bridge.name.data());

	ret = cmdExecSync("iptables -t filter -A FORWARD -i %s ! -o %s -j ACCEPT",
					  nCfg.bridge.name.data(), nCfg.bridge.name.data());
	ret = cmdExecSync("iptables -t filter -A FORWARD ! -i %s -o %s -j ACCEPT",
					  nCfg.bridge.name.data(), nCfg.bridge.name.data());

	if (nCfg.ccFlag == true) {
		ret = cmdExecSync("iptables -t filter -A FORWARD -i %s -o %s -j ACCEPT",
						  nCfg.bridge.name.data(), nCfg.bridge.name.data());
	}
	else {
		ret = cmdExecSync("iptables -t filter -A FORWARD -i %s -o %s -j DROP",
				          nCfg.bridge.name.data(), nCfg.bridge.name.data());
	}

	sharkLog(SHARK_LOG_DEBUG, "bridgeIptablesInit successfully\n");
	return ret;
}

int shark::GlobalNetwork::bridgeIptablesExit() {
	int ret = 0;

	if (nCfg.ccFlag == true) {
		ret = cmdExecSync("iptalbes -t filter -D FORWARD -i %s -o %s -j ACCEPT", \
						  nCfg.bridge.name.data(), nCfg.bridge.name.data());
	}
	else {
		ret = cmdExecSync("iptalbes -t filter -D FORWARD -i %s -o %s -j DROP", \
						  nCfg.bridge.name.data(), nCfg.bridge.name.data());
	}


	ret = cmdExecSync("iptables -t nat -D POSTROUTING -s %u.%u.%u.%u/%u ! -o %s -j MASQUERADE",
					 nCfg.bridge.addr.value.array[0], nCfg.bridge.addr.value.array[1],
					 nCfg.bridge.addr.value.array[2], nCfg.bridge.addr.value.array[3],
					 nCfg.bridge.addr.mask, nCfg.bridge.name.data());
	ret = cmdExecSync("iptables -t nat -D PREROUTING -m addrtype --dst-type LOCAL -j %s", sharkChain);
	ret = cmdExecSync("iptables -t nat -D OUTPUT -m addrtype --dst-type LOCAL -j %s", sharkChain);
	ret = cmdExecSync("iptables -t nat -D %s", sharkChain);

	ret = cmdExecSync("iptables -t filter -D FORWARD -i %s ! -o %s -j ACCEPT",
					  nCfg.bridge.name.data(), nCfg.bridge.name.data());
	ret = cmdExecSync("iptables -t filter -D FORWARD ! -i %s -o %s -j ACCEPT",
					  nCfg.bridge.name.data(), nCfg.bridge.name.data());

	flagSet("/proc/sys/net/ipv4/ip_forward", originalIpv4ForwardFlag);

	sharkLog(SHARK_LOG_DEBUG, "bridgeIptablesExit successfully\n");
	return ret;
}

int shark::GlobalNetwork::bridgeInit() {
	int ret = 0;

	ret = cmdExecSync("ip link add name %s type bridge", nCfg.bridge.name.data());
	ret = cmdExecSync("ip link set %s up", nCfg.bridge.name.data());
	ret = cmdExecSync("ip addr add %d.%d.%d.%d/%d broadcast %d.%d.%d.%d dev %s",
					  nCfg.bridge.addr.value.array[0], nCfg.bridge.addr.value.array[1],
					  nCfg.bridge.addr.value.array[2], nCfg.bridge.addr.value.array[3],
					  nCfg.bridge.addr.mask, nCfg.bridge.addr.bdValue.array[0],
					  nCfg.bridge.addr.bdValue.array[1], nCfg.bridge.addr.bdValue.array[2],
					  nCfg.bridge.addr.bdValue.array[3], nCfg.bridge.name.data());

	ret = bridgeIptablesInit();

	sharkLog(SHARK_LOG_DEBUG, "bridgeInit successfully\n");
	return ret;
}

int shark::GlobalNetwork::bridgeExit() {
	int ret = 0;

	ret = bridgeIptablesExit();

	ret = cmdExecSync("ip link set %s down", nCfg.bridge.name.data());
	ret = cmdExecSync("ip link del dev bridge", nCfg.bridge.name.data());

	sharkLog(SHARK_LOG_DEBUG, "Bridge exit successfully\n");
	return ret;
}

shark::GlobalNetwork::GlobalNetwork(NetworkConfig &cfg):
nCfg(cfg) {
	int ret = 0;

	if (nCfg.enable == false) {
		sharkLog(SHARK_LOG_DEBUG, "GlobalNetwork construct finished, disabled\n");
		return;
	}

	switch(nCfg.type) {
	case NETWORK_TYPE::NETWORK_BRIDGE:
		ret = bridgeInit();
		break;
	default:
		sharkLog(SHARK_LOG_ERR, "Wrong network type\n");
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "GlobalNetwork construct successfully, ret:%d\n", ret);
}

shark::GlobalNetwork::~GlobalNetwork() {
	int ret = 0;

	if (nCfg.enable == false) {
		sharkLog(SHARK_LOG_DEBUG, "Network destruct finished, disabled\n");
		return;
	}

	switch(nCfg.type) {
	case NETWORK_TYPE::NETWORK_BRIDGE:
		ret = bridgeExit();
		break;
	default:
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "Network destruct successfully, ret:%d\n", ret);
}
