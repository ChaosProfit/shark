/*
 * log.hpp
 *
 *  Created on: 2018年2月3日
 *      Author: luguanglong
 */

#ifndef LOG_HPP_
#define LOG_HPP_

#pragma GCC diagnostic ignored "-Wwrite-strings"

#define SHARK_LOG_ERR	1
#define SHARK_LOG_INFO	2
#define SHARK_LOG_DEBUG	3

void sharkPrint(int debugMode, char *file, int line, const char *format, ...);
#define sharkLog(mode, ...) sharkPrint(mode, __FILE__, __LINE__, __VA_ARGS__)

int sharkLogInit(char *log_file_path);

#endif /* LOG_HPP_ */
