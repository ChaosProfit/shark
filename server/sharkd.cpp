/*
 * shark.cpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#include <string.h>

#include "sharkd.hpp"
#include "utils/sepoll.hpp"
#include "utils/daemon.hpp"
#include "utils/log.hpp"

int shark::Sharkd::run(){
	while(1){
		sleep(3);
	}

	return 0;
}

shark::Sharkd::Sharkd(){
	dirInit();

	gConfig = new GlobalConfig();
	containerPool = new ContainerPool(*gConfig);
	cliServer = new CliServer(*containerPool);
	gNetwork = new GlobalNetwork(gConfig->getConfig()->nType);

	sharkLog(SHARK_LOG_INFO, "Sharkd construct successfully\n");
}

shark::Sharkd::~Sharkd(){
	delete containerPool;
	delete cliServer;
	delete gNetwork;

	sharkLog(SHARK_LOG_INFO, "Sharkd destruct successfully");
}

int main(int argc, char *argv[]){
	sharkLogInit("/run/shark/sharkd.log");

	daemonInit();
	shark::Sharkd	*sharkd = new shark::Sharkd();

	sharkdPtrStore(SHARKD_PTR_SAVE, sharkd);

	sharkd->run();

	delete sharkd;
	return 0;
}
