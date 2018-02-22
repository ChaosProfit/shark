/*
 * json.cpp
 *
 *  Created on: 2018年2月10日
 *      Author: luguanglong
 */

#include "simpleJson.hpp"

void shark::JsonString::begin(){
	jsonStr.append("{");
}

void shark::JsonString::end(){
	if(jsonStr.size() == 0){
		return;
	}

	jsonStr.pop_back();
	jsonStr.append("}");
}

void shark::JsonString::addMember(const char *name, std::string &value){
	jsonStr.append("\"");
	jsonStr.append(name);
	jsonStr.append("\"");
	jsonStr.append(":");
	jsonStr.append("\"");
	jsonStr.append(value);
	jsonStr.append("\"");
	jsonStr.append(",");
}

void shark::JsonString::addMember(const char *name, int value){
	jsonStr.append("\"");
	jsonStr.append(name);
	jsonStr.append("\"");
	jsonStr.append(":");
	jsonStr.append(std::to_string(value));
	jsonStr.append(",");
}
