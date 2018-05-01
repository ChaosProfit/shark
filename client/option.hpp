/*
 * options.hpp
 *
 *  Created on: 2018年2月11日
 *      Author: luguanglong
 */

#ifndef OPTION_HPP_
#define OPTION_HPP_

#include <string>

namespace shark {
enum class COMMAND_TYPE{
	COMMAND_BLANK = 0,
	COMMAND_CREATE,
	COMMAND_DELETE,
	COMMAND_EXEC,
	COMMAND_LIST,
	COMMAND_MAX
};

typedef struct {
	std::string ipAddr;
	std::string netBandwidth;
	std::string mountPath;
	std::string mountType;
	std::string cpuSelect;
	std::string cpuQuota;
	std::string memQuota;
}Config;

typedef struct {
	std::string id;
	COMMAND_TYPE type;
	Config cfg;
	std::string execCmd;
} Command;

class Option {
 public:
	const char *process(int argc, char *argv[]);
	Command& getCmd() {
		return cmd;
	}

	std::string& getCmdJson() {
		return jsonCmd;
	}

 private:
	int listProcess(char *data);
	int printHelp();

	std::string cmdGenerate(Command &opt);

	Command cmd;
	std::string jsonCmd;
};
}  // namespace shark

#endif /* OPTION_HPP_ */
