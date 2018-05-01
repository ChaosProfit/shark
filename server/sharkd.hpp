/*
 * shark.hpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#ifndef SHARKD_HPP_
#define SHARKD_HPP_

#include <string>
#include <memory>

#include "network/globalNetwork.hpp"
#include "config/globalConfig.hpp"
#include "container/container.hpp"
#include "container/containerPool.hpp"
#include "cliServer.hpp"
#include "utils/threadQueue.hpp"

namespace shark {
class Sharkd {
 public:
	Sharkd();
	~Sharkd();

	int run();
 private:
	GlobalConfig *gConfig = nullptr;
	SharkConfig sCfg;
	std::unique_ptr<ContainerPool> containerPool = nullptr;
	//	ContainerPool *containerPool = nullptr;
	std::unique_ptr<CliServer> cliServer = nullptr;
//	CliServer *cliServer = nullptr;
//	GlobalNetwork *gNetwork = nullptr;
	std::unique_ptr<GlobalNetwork> gNetwork = nullptr;
};
}  // namespace shark

#endif /* SHARK_HPP_ */
