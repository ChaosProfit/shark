/*
 * shark.hpp
 *
 *  Created on: Jan 27, 2018
 *      Author: luguanglong
 */

#ifndef SHARK_SHARK_HPP_
#define SHARK_SHARK_HPP_

#include <getopt.h>

#include <string>

#include "option.hpp"
#include "cliClient.hpp"

namespace shark{

	class Shark{
	public:
		int process(int argc, char *argv[]);
		Shark();
		~Shark();

	private:
		int retProcess(int type, const char *ret);

		int timeOutProcess();
		CliClient *cliClient = NULL;
		Option *option = NULL;
	};
}

#endif /* SHARK_SHARK_HPP_ */
