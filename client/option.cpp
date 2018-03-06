/*
 * options.cpp
 *
 *  Created on: 2018年2月11日
 *      Author: luguanglong
 */

#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include <iostream>
#include <string>

#include "utils/exceptions.hpp"
#include "utils/log.hpp"
#include "option.hpp"
#include "utils/json.hpp"

static struct option sharkOptions[] = {
												{"command",  required_argument, 0, 'c'},
												{"exec-cmd",  required_argument, 0, 'e'},
												{"id",  required_argument, 0, 0},
												{"addr",  required_argument, 0, 0},
												{"net-bandwidth",  required_argument, 0, 0},
												{"mount-type",  required_argument, 0, 0},
												{"cpu-set",  required_argument, 0, 0},
												{"cpu-quota",  required_argument, 0, 0},
												{"mem-quota",  required_argument, 0, 0},
												{0,	0,	0,	0}
											};

const char *shark::Option::process(int argc, char *argv[]){
	int optionIndex = 0;
	int cnt = 0;

	if(argc == 1){
		printHelp();

		sharkLog(SHARK_LOG_DEBUG, "optionProcess successfully\n");
		return NULL;
	}

	while((cnt = getopt_long(argc, argv, "c:e:", sharkOptions, &optionIndex)) != -1){
		switch(cnt){
		case 0:
			if (strcmp(sharkOptions[optionIndex].name, "id") == 0){
				cmd.id = optarg;
			}
			else if(strcmp(sharkOptions[optionIndex].name, "addr") == 0){
				cmd.cfg.ipAddr = optarg;
			}
			else if(strcmp(sharkOptions[optionIndex].name, "net-bandwidth") == 0){
				cmd.cfg.netBandwidth = optarg;
			}
			else if(strcmp(sharkOptions[optionIndex].name, "cpu-select") == 0){
				cmd.cfg.cpuSelect = optarg;
			}
			else if(strcmp(sharkOptions[optionIndex].name, "cpu-quota") == 0){
					cmd.cfg.cpuQuota = optarg;
				}
			else if(strcmp(sharkOptions[optionIndex].name, "mem-quota") == 0){
				cmd.cfg.memQuota = optarg;
			}
			else if(strcmp(sharkOptions[optionIndex].name, "mount-type") == 0){
				cmd.cfg.mountType = optarg;
			}
			else{
 				sharkLog(SHARK_LOG_ERR, "unknown Options\n");
				throw new SharkException("unknown Options");
			}

			break;

		case 'c':
			if(strcmp(optarg, "exec") == 0){
				cmd.type = COMMAND_EXEC;
			}
			else if(strcmp(optarg, "create") == 0){
				cmd.type = COMMAND_CREATE;
			}
			else if(strcmp(optarg, "list") == 0){
				cmd.type = COMMAND_LIST;
			}
			else if(strcmp(optarg, "delete") == 0){
				cmd.type = COMMAND_DELETE;
			}
			else{
				sharkLog(SHARK_LOG_ERR, "unknown Command Type\n");
				throw new SharkException("unknown Command Type");
			}

			break;

		case 'e':
			cmd.execCmd = optarg;
			break;

		default:
			printHelp();
			return NULL;
		}
	}

	jsonCmd = cmdGenerate(cmd);

	sharkLog(SHARK_LOG_DEBUG, "jsonCmd:%s", jsonCmd.data());
	sharkLog(SHARK_LOG_DEBUG, "optionProcess successfully\n");

	return jsonCmd.data();
}

int shark::Option::printHelp(){
	std::cout << "-c --command command type. The supported cmd type: exec create list delete" <<std::endl;
	std::cout << "--id the container Id." <<std::endl;
	std::cout << "-e --execCmd the name of exec bin, it does not support paras now." <<std::endl;
	std::cout << "--addr the ip addr of the container. It will be allocated by default if not set" <<std::endl;
	std::cout << "--net-bandwidth the bandwidth of the net. Eg:1kbps. There is no limit by default." <<std::endl;
	std::cout << "--mount-type the mount type: share、private、master/slave、unbindable. It is the type of share by default." <<std::endl;
	std::cout << "--cpu-select select which cpu to run. Eg:1. There is no limit by default." <<std::endl;
	std::cout << "--cpu-quota set the ratio of cpu usage. Eg: 50%. If you want to use two cpus, set 200%" <<std::endl;
	std::cout << "--mem-quota set the quota of memory. The unit is kbytes, mbytes and gbytes." <<std::endl;

	return 0;
}

std::string shark::Option::cmdGenerate(Command &opt){
	nlohmann::json cmd;

	cmd["type"] = (int)opt.type;
	cmd["id"] = opt.id;
	cmd["execCmd"] = opt.execCmd;
	cmd["ipAddr"] = opt.cfg.ipAddr;
	cmd["netBandwidth"] = opt.cfg.netBandwidth;
	cmd["mountType"] = opt.cfg.mountType;
	cmd["cpuSelect"] = opt.cfg.cpuSelect;
	cmd["cpuQuota"] = opt.cfg.cpuQuota;
	cmd["memQuota"] = opt.cfg.memQuota;

	sharkLog(SHARK_LOG_DEBUG, "cmd generate successfully\n");
	return cmd.dump();
}
