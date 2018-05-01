/*
 * shark.cpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#include <string.h>

#include <chrono>
#include <thread>
#include <memory>

#include "sharkd.hpp"
#include "utils/sepoll.hpp"
#include "utils/daemon.hpp"
#include "utils/log.hpp"
#include "utils/misc.hpp"

int shark::Sharkd::run() {
	while(1) {
		std::this_thread::sleep_for(std::chrono::seconds(3));
	}

	return 0;
}

shark::Sharkd::Sharkd() {
	std::shared_ptr<GlobalConfig> cfgReader = std::make_shared<GlobalConfig>();

	sCfg = cfgReader->getConfig();
	gNetwork = std::make_unique<GlobalNetwork>(sCfg.net);
	containerPool = std::make_unique<ContainerPool>(sCfg);
	cliServer = std::make_unique<CliServer>(*containerPool);

	sharkLog(SHARK_LOG_INFO, "Sharkd construct successfully\n");
}

shark::Sharkd::~Sharkd() {

	sharkLog(SHARK_LOG_INFO, "Sharkd destruct successfully");
}

int main(int argc, char *argv[]) {

	dirInit();
	sharkLogInit("/run/shark/sharkd.log");
	daemonInit();

	std::unique_ptr<shark::Sharkd> sharkd = std::make_unique<shark::Sharkd>();
	sharkd->run();

	return 0;
}
