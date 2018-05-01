/*
 * daemon.hpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#ifndef UTILS_DAEMON_HPP_
#define UTILS_DAEMON_HPP_

extern int daemonInit();
extern int dirInit();

enum class SHARKD_PTR_OPERATE {
	SHARKD_PTR_SAVE = 1,
	SHARKD_PTR_GET
};

shark::Sharkd *sharkdPtrStore(SHARKD_PTR_OPERATE type, shark::Sharkd *ptr);

#endif /* UTILS_DAEMON_HPP_ */
