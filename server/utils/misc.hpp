/*
 * misc.hpp
 *
 *  Created on: 2018年2月10日
 *      Author: luguanglong
 */

#ifndef UTILS_MISC_HPP_
#define UTILS_MISC_HPP_

int sharkdPidUpdate();
int rmDir(const char *dirPath);
int cmdExecSync(const char *format, ...);
int dirInit();

#endif /* UTILS_MISC_HPP_ */
