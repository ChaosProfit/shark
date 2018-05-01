/*
 * config.hpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */

#ifndef CONFIG_GLOBALCONFIG_HPP_
#define CONFIG_GLOBALCONFIG_HPP_

#include <netinet/in.h>
#include <arpa/inet.h>

#include <memory>
#include <string>

namespace shark {
typedef enum {
	NETWORK_NONE = 0,
	NETWORK_BRIDGE,
} NETWORK_TYPE;

union IntegerArrary {
	unsigned int integer;
	unsigned char array[4];
};
typedef struct {
	std::string str;
	union IntegerArrary value;
	union IntegerArrary bdValue;
	unsigned int mask;
}Ipv4Addr;

typedef struct {
	std::string name;
	Ipv4Addr addr;
} Bridge;

typedef struct {
	bool enable;
	NETWORK_TYPE type;
	bool	ccFlag;
	Bridge	bridge;
} NetworkConfig;

typedef struct {
	NetworkConfig net;
} SharkConfig;

int ipv4AddrPreprocess(Ipv4Addr &addr);

class GlobalConfig {
 public:
	GlobalConfig();
	~GlobalConfig();

	int optionProcess(int argc, char *argv[]);
	SharkConfig getConfig();
 private:
	SharkConfig *gConfig;
	std::string configFile = "/etc/shark/shark.conf";

	int configRead();
	int configLineProcess(char *line);

	int defaultCfgInit();
};
}  // namespace shark


#endif /* CONFIG_GLOBALCONFIG_HPP_ */
