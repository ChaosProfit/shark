/*
 * config.cpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */
#include <sys/socket.h>

#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include <iostream>
#include <fstream>
#include <regex>

#include "globalConfig.hpp"
#include "utils/log.hpp"

static struct option sharkOptions[] =  {
												 {"config",  required_argument, 0, 'c'},
												 {0,	0,	0,	0}
											};

shark::GlobalConfig::GlobalConfig() {
	gConfig = new SharkConfig();

	configRead();
	defaultCfgInit();

	sharkLog(SHARK_LOG_DEBUG, "GlobalConfig construct successfully\n");
}

shark::GlobalConfig::~GlobalConfig() {
	delete gConfig;

	sharkLog(SHARK_LOG_DEBUG, "GlobalConfig destruct successfully\n");
}

int shark::GlobalConfig::defaultCfgInit() {
	if (gConfig->net.bridge.name.size() == 0) {
		gConfig->net.bridge.name = "shark0";
	}

	sharkLog(SHARK_LOG_DEBUG, "defaultCfgInit finished\n");
	return 0;
}

shark::SharkConfig shark::GlobalConfig::getConfig() {
	return *gConfig;
}

int shark::GlobalConfig::optionProcess(int argc, char *argv[]) {
	int cnt = 0;
	int optionIndex = 0;

	while((cnt = getopt_long(argc, argv, "c:", sharkOptions, &optionIndex)) != -1) {
		switch(cnt) {
		case 'c':
			configFile = optarg;
			break;

		default:
			sharkLog(SHARK_LOG_ERR, "unknown options:%c\n", cnt);
			return -1;
		}
	}

	sharkLog(SHARK_LOG_DEBUG, "GlobalConfig construct successfully\n");
	return 0;
}

int shark::GlobalConfig::configLineProcess(char *line) {
	std::regex reg("([\\w-]+)([= ]+)([\\w\\d./]+)(\\n)");
	std::smatch match;

	std::string input(line);
	bool found = regex_match(input, match, reg);
	if (found == false) {
		sharkLog(SHARK_LOG_ERR, "Failed to recognize %s\n", line);
		return -1;
	}

	if (match.size() != 5) {
		sharkLog(SHARK_LOG_ERR, "Failed to recognize %s, size:%d\n", line, match.size());
		return -1;
	}

	std::string key = match[1].str();
	std::string value = match[3].str();

	if (key.compare("network-enable") == 0) {
		if (value.compare("True") == 0) {
			gConfig->net.enable = true;
		} else {
			gConfig->net.enable = false;
		}
	} else if (key.compare("network-type") == 0) {
		if (value.compare("bridge") == 0) {
			gConfig->net.type = NETWORK_TYPE::NETWORK_BRIDGE;
		}
	} else if (key.compare("container-communicate") == 0) {
		if (value.compare("True") == 0) {
			gConfig->net.ccFlag = true;
		}
	} else if (key.compare("bridge-ip") == 0) {
		gConfig->net.bridge.addr.str = value;
		ipv4AddrPreprocess(gConfig->net.bridge.addr);
	}

	return 0;
}

int shark::GlobalConfig::configRead() {
	char readBuf[256] =  {0};
	std::ifstream config_file;
	std::string tm;
	config_file.open(configFile.data());
	while(config_file.getline(readBuf, sizeof(readBuf))){
		if ((readBuf[0] == '#') || (readBuf[0] == ' ')) {
			continue;
		}

		configLineProcess(readBuf);
		memset(readBuf, 0, 256);
	}

	config_file.close();
	sharkLog(SHARK_LOG_DEBUG, "configRead successfully\n");
	return 0;
//
//	FILE *fPtr = fopen(configFile.data(), "r");
//
//	if (fPtr == nullptr) {
//		sharkLog(SHARK_LOG_ERR, "open %s failed\n", configFile.data());
//		return -1;
//	}
//
//	while(fgets(readBuf, 256, fPtr) != nullptr) {
//		if ((readBuf[0] == '#') || (readBuf[0] == ' ')) {
//			continue;
//		}
//
//		configLineProcess(readBuf);
//		memset(readBuf, 0, 256);
//	}
//
//	fclose(fPtr);
//
//	sharkLog(SHARK_LOG_DEBUG, "configRead successfully\n");
//	return 0;
}

int shark::ipv4AddrPreprocess(Ipv4Addr &addr) {
	int ret = sscanf(addr.str.data(), "%u.%u.%u.%u/%u",
										(unsigned int*)&addr.value.array[0], (unsigned int*)&addr.value.array[1],
										(unsigned int*)&addr.value.array[2], (unsigned int*)&addr.value.array[3],
										&addr.mask);

	if (ret == EOF) {
		sharkLog(SHARK_LOG_ERR, "brIpv4Process failed, ipStr:%s\n", addr.str.data());
		return -1;
	}

	if (addr.mask > 32) {
		sharkLog(SHARK_LOG_ERR, "brIpv4Process failed, mask:%d\n", addr.mask);
		return -1;
	}

	for(int index = 0; index < 4; index++) {
		if (addr.value.array[index] > 255) {
			sharkLog(SHARK_LOG_ERR, "brIpv4Process failed, ipStr:%s\n", addr.str.data());
			return -1;
		}
	}

	addr.bdValue = addr.value;

	for(int index = addr.mask; index >= 0; index--) {
		addr.bdValue.integer |= (0x1 << (32 - index));
	}

	sharkLog(SHARK_LOG_DEBUG, "mask:%d\n",
			addr.mask);

	sharkLog(SHARK_LOG_DEBUG, "addr0:%d, addr1:%d, addr2:%d, addr3:%d\n",
			addr.value.array[0], addr.value.array[1], addr.value.array[2], addr.value.array[3]);

	sharkLog(SHARK_LOG_DEBUG, "br0:%d, br1:%d, br2:%d, br3:%d\n",
			addr.bdValue.array[0], addr.bdValue.array[1], addr.bdValue.array[2], addr.bdValue.array[3]);

	sharkLog(SHARK_LOG_DEBUG, "brIpv4Process successfully\n");
	return 0;
}
