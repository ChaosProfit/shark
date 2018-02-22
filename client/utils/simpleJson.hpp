/*
 * simpleJson.hpp
 *
 *  Created on: 2018年2月10日
 *      Author: luguanglong
 */

#ifndef SIMPLEJSON_HPP_
#define SIMPLEJSON_HPP_

#include <string>

namespace shark{
	class JsonString{
	public:
		void addMember(const char *name, std::string &value);
		void addMember(const char *name, int value);
		void begin();
		void end();

		std::string data(){
			return jsonStr;
		};
	private:
		std::string jsonStr;
	};
}


#endif /* SIMPLEJSON_HPP_ */
