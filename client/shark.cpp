/*
 * shark.cpp
 *
 *  Created on: Jan 27, 2018
 *      Author: luguanglong
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "utils/exceptions.hpp"
#include "shark.hpp"
#include "utils/log.hpp"
#include "option.hpp"
#include "utils/json.hpp"

shark::Shark::Shark() {
	cliClient = new CliClient();
	option = new Option();
}

shark::Shark::~Shark() {
	delete cliClient;
	delete option;
}

int shark::Shark::timeOutProcess() {
	fprintf(stdout, "time out\n");
	return 0;
}

int shark::Shark::retProcess(COMMAND_TYPE type, const char* ret) {
	sharkLog(SHARK_LOG_DEBUG, "ret:%s\n", ret);
	nlohmann::json retJson = nlohmann::json::parse(std::string(ret));
	nlohmann::json dataJson;

	if ((retJson["ret"].empty() == 1) || (retJson["ret"].get<int>() < 0)) {
		fprintf(stdout, "Process failed\n");
		return -1;
	}

	switch(type) {
	case COMMAND_TYPE::COMMAND_CREATE:
		break;

	case COMMAND_TYPE::COMMAND_EXEC:
		break;

	case COMMAND_TYPE::COMMAND_DELETE:
		break;

	case COMMAND_TYPE::COMMAND_LIST:
		if (retJson["data"].empty() == 1) {
			std::cout << "List process failed\n";
			return -1;
		}

		dataJson = nlohmann::json::parse(retJson["data"].get<std::string>());
		sharkLog(SHARK_LOG_DEBUG, "dataType:%d\n", dataJson.type());

		std::cout << "Id" << "\n";
		for (nlohmann::json::iterator it = dataJson.begin(); it != dataJson.end(); ++it)  {
			std::string tmp = (*it).dump();
			std::cout << tmp.substr(1, tmp.size() - 2) << "\n";
		}
		return 0;

	default:
		break;
	}

	return 0;
}

int shark::Shark::process(int argc, char *argv[]) {
	const char *dataToSend = option->process(argc, argv);

	if (dataToSend != nullptr) {
		sharkLog(SHARK_LOG_DEBUG, "result:%s\n", dataToSend);
		const char *dataPtr = (const char *)cliClient->dataSend(dataToSend, strlen(dataToSend));
		if (dataPtr == nullptr) {
			sharkLog(SHARK_LOG_DEBUG, "data send failed\n");
			return -1;
		}

		Command& cmd = option->getCmd();
		sharkLog(SHARK_LOG_DEBUG, "ret:%s\n", dataPtr);
		retProcess(cmd.type, dataPtr);
	}

	sharkLog(SHARK_LOG_DEBUG, "Shark process successfully\n");
	return 0;
}

int main(int argc, char *argv[]) {
	sharkLogInit("/run/shark/shark.log");

	shark::Shark shark;

	shark.process(argc, argv);

	sharkLog(SHARK_LOG_DEBUG, "Shark client exit successfully\n");
	return 0;
}
