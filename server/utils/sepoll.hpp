/*
 * sepoll.h
 *
 *  Created on: 2017年10月10日
 *      Author: luguanglong
 */

#ifndef SEPOLL_H_
#define SEPOLL_H_

namespace shark  {
	class Sepoll {
	public:
		Sepoll();
		~Sepoll();

		int addPort(int i_port);
		int delPort(int i_port);
		int wait(int i_socket_fd, int timeout);

	private:
		static const int EPOLL_MAX_EVENTS = 3;
		struct	epoll_event *pst_epoll_events;
		int epollFd;
	};
};

#endif /* SEPOLL_H_ */
