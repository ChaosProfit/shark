/*
 * containerPool.cpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sched.h>

#include "containerPool.hpp"
#include "utils/exceptions.hpp"

#include <list>
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "utils/log.hpp"
#include "utils/json.hpp"

shark::ContainerPool::ContainerPool(GlobalConfig &gCfg):
gConfig(gCfg){

	sharkLog(SHARK_LOG_INFO, "ContainerPool construct successfully\n");
	return;
};

shark::ContainerPool::~ContainerPool(){
	for(auto container=clist.begin(); container != clist.end(); container++){
		delete *container;
		clist.remove(*container);
	}

	sharkLog(SHARK_LOG_INFO, "ContainerPool destruct successfully\n");
	return;
}

std::string shark::ContainerPool::retToJson(int ret){
	nlohmann::json retJson;

	retJson["ret"] = ret;

	sharkLog(SHARK_LOG_DEBUG, "retJson:%s\n", retJson.dump().data());
	return retJson.dump();
}

std::string shark::ContainerPool::retToJson(std::string data){
	nlohmann::json retJson;

	retJson["ret"] = 0;
	retJson["data"] = data;

	sharkLog(SHARK_LOG_DEBUG, "retJson:%s\n", retJson.dump().data());
	return retJson.dump();
}

std::string shark::ContainerPool::listCmdProcess(){
	std::string retStr;

	std::list<std::string> slist;
	auto clist = listContainer();

	for(auto c=clist.begin(); c != clist.end(); c++){
		slist.push_back((*c)->getId());
		sharkLog(SHARK_LOG_DEBUG, "Id:%s\n",(*c)->getId().data());
	}

	nlohmann::json retJson(slist);
	sharkLog(SHARK_LOG_DEBUG, "json:%s\n", retJson.dump().data());

	return retJson.dump();
}

std::string shark::ContainerPool::cmdProcess(Command *cmd){
	int ret = 0;
	std::string retStr;
	std::string data;

	switch(cmd->type){
	case COMMAND_CREATE:
		ret = createContainer(*cmd);
		retStr = retToJson(ret);

		sharkLog(SHARK_LOG_DEBUG, "create_ret:%s\n", retStr.data());
		return retStr;

	case COMMAND_DELETE:
			ret = delContainer(*cmd);
			retStr = retToJson(ret);

			sharkLog(SHARK_LOG_DEBUG, "create_ret:%s\n", retStr.data());
			return retStr;

	case COMMAND_EXEC:
		ret = execContainer(*cmd);
		retStr = retToJson(ret);

		sharkLog(SHARK_LOG_DEBUG, "exec_ret:%s\n", retStr.data());
		return retStr;

	case COMMAND_LIST:
		data = listCmdProcess();
		retStr = retToJson(data);

		sharkLog(SHARK_LOG_DEBUG, "list_ret:%s\n", retStr.data());
		return retStr;

	default:
		sharkLog(SHARK_LOG_ERR, "unknown command type\n");
		throw new SharkException("unknown Type");
		return retStr;
	}

	return retStr;
}

int shark::ContainerPool::execContainer(struct Command &cmd){
	auto c = getContainer(cmd.cfg.id);

	if(c == NULL){
		sharkLog(SHARK_LOG_ERR, "execContainer failed\n");
		return -1;
	}

	c->cmdSend(cmd.cfg.execCmd);

	sharkLog(SHARK_LOG_DEBUG, "execContainer successfully\n");
	return 0;
};

int shark::ContainerPool::createContainer(struct Command &cmd){
	int ret = 0;
	ContainerConfig &cConfig = cmd.cfg;

	cConfig.nType = gConfig.getConfig()->nType;
	Container *c = new Container(cmd.cfg);

	ret = c->start();
	if(ret < 0){
		sharkLog(SHARK_LOG_ERR, "container start failed\n");
		return ret;
	}

	if(getContainer(c->getId()) != NULL){
		sharkLog(SHARK_LOG_ERR, "addContainer %s faild\n", c->getId().data());
		return -1;
	}

	clist.push_back(c);

	if(cmd.cfg.execCmd.size() > 0){
		c->cmdSend(cmd.cfg.execCmd);
	}

	sharkLog(SHARK_LOG_DEBUG, "createContainer %s successfully\n", c->getId().data());
	return ret;
}

int shark::ContainerPool::delContainer(struct Command &cmd){
	Container *cPtr = getContainer(cmd.cfg.id);
	if(cPtr == NULL){
		sharkLog(SHARK_LOG_ERR, "delContainer %s faild\n", cmd.cfg.id.data());
		return -1;
	}

	clist.remove(cPtr);
	delete cPtr;

	sharkLog(SHARK_LOG_DEBUG, "delContainer %s successfully\n", cmd.cfg.id.data());
	return 0;
}

shark::Container *shark::ContainerPool::getContainer(std::string &id){
	for(auto c = clist.begin(); c != clist.end(); c++){
		if(id == (*c)->getId()){
			sharkLog(SHARK_LOG_DEBUG, "getContainer %s successfully\n", id.data());
			return *c;
		}
	}

	sharkLog(SHARK_LOG_ERR, "getContainer %s faild\n", id.data());
	return NULL;
}

std::list<shark::Container *>& shark::ContainerPool::listContainer(){
	return clist;
}
