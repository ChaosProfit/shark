/*
 * config.cpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include <iostream>
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
//	std::regex reg("([\\w\\d-_.]*)([ ]+=[ ]+)([\\w\\d-_.]*)");
	std::regex reg("([\\w-]+)([= ]+)([\\w\\d.]+)(\\n)");
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

	if(key.compare("network-type") == 0){
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
		gConfig->net.bridgeIp = value;
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
