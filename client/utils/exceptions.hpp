/*
 * exceptions.hpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#ifndef UTILS_EXCEPTIONS_HPP_
#define UTILS_EXCEPTIONS_HPP_

#include <string>
#include <exception>

class SharkException : public std::exception{
public:
	SharkException(std::string input) : msg(input){
	};

	SharkException(const char *input) : msg(input){
	};

	const char* what(){return msg.data();}
private:
	std::string msg;
};

#endif /* UTILS_EXCEPTIONS_HPP_ */
