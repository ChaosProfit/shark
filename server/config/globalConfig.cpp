/*
 * config.cpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include <regex>

#include "globalConfig.hpp"
#include "utils/log.hpp"

static struct option sharkOptions[] = {
												{"config",  required_argument, 0, 'c'},
												{0,	0,	0,	0}
											};

shark::GlobalConfig::GlobalConfig(){
	gConfig = new SharkConfig();

	configRead();

	sharkLog(SHARK_LOG_DEBUG, "GlobalConfig construct successfully\n");
}

shark::GlobalConfig::~GlobalConfig(){
	delete gConfig;

	sharkLog(SHARK_LOG_DEBUG, "GlobalConfig destruct successfully\n");
}

shark::SharkConfig* shark::GlobalConfig::getConfig(){
	return gConfig;
}

int shark::GlobalConfig::optionProcess(int argc, char *argv[]){
	int cnt = 0;
	int optionIndex = 0;

	while((cnt = getopt_long(argc, argv, "c:", sharkOptions, &optionIndex)) != -1){
		switch(cnt){
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

int shark::GlobalConfig::configLineProcess(char *line){
	std::regex reg("([\\w-]+)([= ]+)([\\w\\d./]+)(\\n)");
	std::smatch match;

	std::string input(line);
	bool found = regex_match(input, match, reg);
	if(found == false){
		sharkLog(SHARK_LOG_ERR, "Failed to recognize %s\n", line);
		return -1;
	}

	if(match.size() != 5){
		sharkLog(SHARK_LOG_ERR, "Failed to recognize %s, size:%d\n", line, match.size());
		return -1;
	}

	std::string key = match[1].str();
	std::string value = match[3].str();

	if(key.compare("network-enable") == 0){
		if(value.compare("True") == 0){
			gConfig->net.enable = true;
		}
		else{
			gConfig->net.enable = false;
		}
	}
	else if(key.compare("network-type") == 0){
		if(value.compare("bridge") == 0){
			gConfig->net.type = NETWORK_BRIDGE;
		}
	}
	else if(key.compare("container-communicate") == 0){
		if(value.compare("True") == 0){
			gConfig->net.ccFlag = true;
		}
	}
	else if(key.compare("bridge-ip") == 0){
		gConfig->net.bridge.addrStr = value;
		brIpv4Process(gConfig->net.bridge);
	}

	return 0;
}

int shark::GlobalConfig::configRead(){
	char readBuf[256] = {0};

	FILE *fPtr = fopen(configFile.data(), "r");

	if(fPtr == NULL){
		sharkLog(SHARK_LOG_ERR, "open %s failed\n", configFile.data());
		return -1;
	}

	while(fgets(readBuf, 256, fPtr) != NULL){
		if((readBuf[0] == '#') || (readBuf[0] == ' ')){
			continue;
		}

		configLineProcess(readBuf);
		memset(readBuf, 0, 256);
	}

	fclose(fPtr);

	sharkLog(SHARK_LOG_DEBUG, "configRead successfully\n");
	return 0;
}

int shark::GlobalConfig::brIpv4Process(Bridge &bridge){

	int ret = sscanf(bridge.addrStr.data(), "%u.%u.%u.%u/%u",
										(unsigned int*)&bridge.addr.array[0], (unsigned int*)&bridge.addr.array[1],
										(unsigned int*)&bridge.addr.array[2], (unsigned int*)&bridge.addr.array[3],
										(unsigned int*)&bridge.addrMask);

	if(ret == EOF){
		sharkLog(SHARK_LOG_ERR, "brIpv4Process failed, ipStr:%s\n", bridge.addrStr.data());
		return -1;
	}

	if(bridge.addrMask > 32){
		sharkLog(SHARK_LOG_ERR, "brIpv4Process failed, mask:%d\n", bridge.addrMask);
		return -1;
	}

	for(int index = 0; index < 4; index++){
		if(bridge.addr.array[index] > 255){
			sharkLog(SHARK_LOG_ERR, "brIpv4Process failed, ipStr:%s\n", bridge.addrStr.data());
			return -1;
		}
	}

	bridge.bdAddr.value = bridge.addr.value;

	for(int index = bridge.addrMask; index >= 0; index--){
		bridge.bdAddr.value |= (0x1 << (32 - index));
	}

	sharkLog(SHARK_LOG_DEBUG, "mask:%d\n",
			bridge.addrMask);

	sharkLog(SHARK_LOG_DEBUG, "addr0:%d, addr1:%d, addr2:%d, addr3:%d\n",
			bridge.addr.array[0], bridge.addr.array[1], bridge.addr.array[2], bridge.addr.array[3]);

	sharkLog(SHARK_LOG_DEBUG, "br0:%d, br1:%d, br2:%d, br3:%d\n",
			bridge.bdAddr.array[0], bridge.bdAddr.array[1], bridge.bdAddr.array[2], bridge.bdAddr.array[3]);

	sharkLog(SHARK_LOG_DEBUG, "brIpv4Process successfully\n");
	return 0;
}
