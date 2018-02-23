/*
 * config.hpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */

#ifndef CONFIG_GLOBALCONFIG_HPP_
#define CONFIG_GLOBALCONFIG_HPP_

#include <memory>

namespace shark{

	typedef enum{
		NETWORK_NONE = 0,
		NETWORK_BRIDGE,
	} NETWORK_TYPE;

	typedef struct{
		NETWORK_TYPE type;
		bool	ccFlag;
		std::string bridgeIp;
	} NetworkConfig;

	typedef struct{
		NetworkConfig net;
	} SharkConfig;

	class GlobalConfig{
	public:
		GlobalConfig();
		~GlobalConfig();

		int optionProcess(int argc, char *argv[]);
		SharkConfig* getConfig();
	private:
		SharkConfig *gConfig;
		std::string configFile = "/etc/shark/shark.conf";

		int configRead();
		int configLineProcess(char *line);
	};
}






#endif /* CONFIG_GLOBALCONFIG_HPP_ */
