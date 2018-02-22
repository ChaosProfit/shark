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

namespace shark{
	typedef enum {
		COMMAND_BLANK = 0,
		COMMAND_CREATE,
		COMMAND_EXEC,
		COMMAND_LIST,
		COMMAND_MAX
	} COMMAND_TYPE;

	typedef struct {
		int	type;
		unsigned long hard;
		unsigned long soft;
	}RLimit;

	struct Config{
		std::string execCmd;
		std::string nameSpace;
		std::string rootfsPath;
		std::list<RLimit> rLimitList;
	};

	struct Command{
		std::string id;
		COMMAND_TYPE type;
		struct Config cfg;
		std::string execCmd;
	};

}

#endif /* LIBCONTAINER_COMMAND_COMMAND_HPP_ */
