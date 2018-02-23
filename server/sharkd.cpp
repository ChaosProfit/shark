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
#include "utils/misc.hpp"

int shark::Sharkd::run(){
	while(1){
		sleep(3);
	}

	return 0;
}

shark::Sharkd::Sharkd(SharkConfig &cfg): sCfg(cfg){

	containerPool = new ContainerPool(sCfg);
	cliServer = new CliServer(*containerPool);
	gNetwork = new GlobalNetwork(sCfg.net);

	sharkLog(SHARK_LOG_INFO, "Sharkd construct successfully\n");
}

shark::Sharkd::~Sharkd(){
	delete containerPool;
	delete cliServer;
	delete gNetwork;

	sharkLog(SHARK_LOG_INFO, "Sharkd destruct successfully");
}

int main(int argc, char *argv[]){
	dirInit();
	sharkLogInit("/run/shark/sharkd.log");

	shark::GlobalConfig *config = new shark::GlobalConfig();

	daemonInit();

	shark::Sharkd	*sharkd = new shark::Sharkd(*(config->getConfig()));
	sharkdPtrStore(SHARKD_PTR_SAVE, sharkd);
	sharkd->run();

	delete sharkd;
	delete config;

	return 0;
}
