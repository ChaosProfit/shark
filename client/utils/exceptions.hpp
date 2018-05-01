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

namespace shark {
class SharkException : public std::exception {
 public:
	explicit SharkException(std::string input) : msg(input) {
	}

	explicit SharkException(const char *input) : msg(input) {
	}

	const char* what() {return msg.data();}
 private:
	std::string msg;
};
}  // namespace shark
#endif /* UTILS_EXCEPTIONS_HPP_ */
