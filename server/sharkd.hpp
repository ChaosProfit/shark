/*
 * shark.hpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#ifndef SHARKD_HPP_
#define SHARKD_HPP_

#include <string>

#include "network/globalNetwork.hpp"
#include "config/globalConfig.hpp"
#include "container/container.hpp"
#include "container/containerPool.hpp"
#include "cliServer.hpp"
#include "utils/threadQueue.hpp"

namespace shark{
	class Sharkd{
	public:
		Sharkd();
		~Sharkd();

		int run();
	private:
		GlobalConfig *gConfig = NULL;
		ContainerPool *containerPool = NULL;
		CliServer *cliServer = NULL;
		GlobalNetwork *gNetwork = NULL;
	};
}

#endif /* SHARK_HPP_ */
