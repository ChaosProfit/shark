/*
 * config.cpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */

#include "globalConfig.hpp"

shark::GlobalConfig::GlobalConfig(){
	gConfig = new SharkConfig();
	gConfig->nType = NETWORK_BRIDGE;
}

shark::GlobalConfig::~GlobalConfig(){
	delete gConfig;
}

shark::SharkConfig* shark::GlobalConfig::getConfig(){
	return gConfig;
}
