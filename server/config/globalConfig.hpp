/*
 * config.hpp
 *
 *  Created on: 2018年2月20日
 *      Author: luguanglong
 */

#ifndef CONFIG_GLOBALCONFIG_HPP_
#define CONFIG_GLOBALCONFIG_HPP_

#include <network/networkConfig.hpp>
#include <memory>


namespace shark{

	typedef struct{
		NETWORK_TYPE nType;
	} SharkConfig;

	class GlobalConfig{
	public:
		GlobalConfig();
		~GlobalConfig();

		SharkConfig* getConfig();
	private:
		SharkConfig *gConfig;
	};
}






#endif /* CONFIG_GLOBALCONFIG_HPP_ */
