/*
 * epoll.cpp
 *
 *  Created on: 2017年10月10日
 *      Author: luguanglong
 */
#include <sys/epoll.h>

#include <string.h>
#include <stdlib.h>

#include "exceptions.hpp"
#include "sepoll.hpp"
#include "log.hpp"

shark::Sepoll::Sepoll() : pst_epoll_events(NULL), epollFd(0){
	epollFd = epoll_create1(0);
	if(epollFd < 0){
		sharkLog(SHARK_LOG_ERR, "listen epoll socket create fail\n");
		throw SharkException("Epoll socket create fail");
	}

	this->pst_epoll_events = (struct epoll_event *)malloc(sizeof(struct epoll_event)*this->EPOLL_MAX_EVENTS);
	if(this->pst_epoll_events == NULL){
		sharkLog(SHARK_LOG_ERR, "epoll events malloc fail\n");
		throw SharkException("Epoll events malloc fail");
	}

	memset(this->pst_epoll_events, 0, sizeof(struct epoll_event)*this->EPOLL_MAX_EVENTS);
	return;
}

shark::Sepoll::~Sepoll(){
	if(this->pst_epoll_events != NULL){
		free(this->pst_epoll_events);
		this->pst_epoll_events = NULL;
	}
}

int shark::Sepoll::addPort(int i_port){
	struct	epoll_event st_listen_epoll_event = {0};

	st_listen_epoll_event.events = EPOLLIN | EPOLLET;
	st_listen_epoll_event.data.fd = i_port;

	if(epoll_ctl(epollFd, EPOLL_CTL_ADD, i_port, &st_listen_epoll_event) < 0){
		sharkLog(SHARK_LOG_ERR, "epoll add port fail\n");
		return -1;
	}

	return 0;
}

int shark::Sepoll::delPort(int i_port){
	struct	epoll_event st_listen_epoll_event = {0};

	st_listen_epoll_event.data.fd = i_port;

	if(epoll_ctl(epollFd, EPOLL_CTL_DEL, i_port, &st_listen_epoll_event) < 0){
		sharkLog(SHARK_LOG_ERR, "epoll del port fail\n");
		return -1;
	}

	return 0;
}

int shark::Sepoll::wait(int i_socket_fd, int timeout){
	int i_num = 0;
	int i_idx = 0;

	i_num = epoll_wait(epollFd, this->pst_epoll_events, this->EPOLL_MAX_EVENTS, timeout);
	if(i_num == 0){
		sharkLog(SHARK_LOG_DEBUG, "epoll wait timeout\n");
		return 0;
	}

	if(i_num == -1){
		sharkLog(SHARK_LOG_DEBUG, "epoll wait failed\n");
		return -1;
	}

	for(i_idx = 0; i_idx < i_num; i_idx++){
		if(this->pst_epoll_events[i_idx].data.fd != i_socket_fd){
			continue;
		}

		if( (this->pst_epoll_events[i_idx].events & EPOLLERR)||
			(this->pst_epoll_events[i_idx].events & EPOLLHUP)||
			!(this->pst_epoll_events[i_idx].events & EPOLLIN)){
			continue;
		}
		break;
	}

	sharkLog(SHARK_LOG_DEBUG, "epoll wait process successfully\n");
	return i_num;
}
