/*
 * config.hpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#ifndef LIBCONTAINER_COMMAND_COMMAND_HPP_
#define LIBCONTAINER_COMMAND_COMMAND_HPP_

#include <pthread.h>

#include <string>
#include <list>
#include <memory>

#include "config/containerConfig.hpp"

namespace shark {
enum class COMMAND_TYPE {
	COMMAND_BLANK = 0,
	COMMAND_CREATE,
	COMMAND_DELETE,
	COMMAND_EXEC,
	COMMAND_LIST,
	COMMAND_MAX
};

struct Command {
	COMMAND_TYPE type;
	ContainerConfig cfg;
};

}  // namespace shark

#endif /* LIBCONTAINER_COMMAND_COMMAND_HPP_ */
