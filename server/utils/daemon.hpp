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

typedef enum {
	SHARKD_PTR_SAVE = 1,
	SHARKD_PTR_GET
} SHARKD_PTR_OPERATE;

shark::Sharkd *sharkdPtrStore(SHARKD_PTR_OPERATE type, shark::Sharkd *ptr);

#endif /* UTILS_DAEMON_HPP_ */
