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

	typedef union{
		unsigned int value;
		unsigned char array[4];
	} Ipv4Addr;

	typedef struct{
		std::string name;
		std::string addrStr;
		Ipv4Addr addr;
		Ipv4Addr bdAddr;
		unsigned char addrMask;
	} Bridge;

	typedef struct{
		bool enable;
		NETWORK_TYPE type;
		bool	ccFlag;
		Bridge	bridge;
	} NetworkConfig;

	typedef struct{
		NetworkConfig net;
	} SharkConfig;

	class GlobalConfig{
	public:
		GlobalConfig();
		~GlobalConfig();

		int brIpv4Process(Bridge &ip);
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
