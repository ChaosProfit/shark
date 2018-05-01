/*
 * daemon.cpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>

#include "utils/log.hpp"
#include "sharkd.hpp"
#include "daemon.hpp"
#include "misc.hpp"

static const char *SharkdParentDir = "/run/shark";

shark::Sharkd *sharkdPtrStore(SHARKD_PTR_OPERATE type, shark::Sharkd *ptr=NULL) {
	static shark::Sharkd *sharkdPtr = NULL;

	if(type == SHARKD_PTR_GET) {
		return sharkdPtr;
	}
	else if (type == SHARKD_PTR_SAVE) {
		sharkdPtr = ptr;
	}

	return NULL;
}


static void sharkExit() {
	int ret = 0;

	shark::Sharkd *sharkd = sharkdPtrStore(SHARKD_PTR_GET);

	sharkLog(SHARK_LOG_INFO, "Sharkd began to exit\n");
	if(sharkd != NULL) {
		delete sharkd;
	}

	ret = rmDir(SharkdParentDir);
	if(ret < 0) {
		sharkLog(SHARK_LOG_ERR, "rm %s failed\n", SharkdParentDir);
	}

	sharkLog(SHARK_LOG_INFO, "Sharkd exit successfully\n");
	exit(0);
}

static void signalProcess(int sig) {
	using namespace::std;
	switch(sig) {
	case SIGPIPE:
		sharkLog(SHARK_LOG_INFO, "catch a SIGPIPE signal\n");
		break;
	case SIGTERM:
		sharkLog(SHARK_LOG_INFO, "catch a SIGTERM signal\n");
		usleep(30000);

		sharkExit();
		break;
	case SIGINT:
		sharkLog(SHARK_LOG_INFO, "catch a SIGINT signal\n");
		sharkExit();
		break;
	case SIGCHLD:
		wait(NULL);
		sharkLog(SHARK_LOG_INFO, "catch a SIGCHLD signal\n");
		break;
	default:
		break;
	}
}

static int signalInit() {
	struct sigaction st_act =  {0};

	st_act.sa_flags = 0;
	st_act.sa_handler = signalProcess;
	if(sigemptyset(&st_act.sa_mask) != 0) {
		sharkLog(SHARK_LOG_ERR, "sigemptyset sigaction error\n");
		return -1;
	}

	if(sigaction(SIGPIPE, &st_act, NULL) != 0) {
		sharkLog(SHARK_LOG_ERR, "SIGPIPE sigaction error\n");
		return -1;
	}

	if(sigaction(SIGCHLD, &st_act, NULL) != 0) {
		sharkLog(SHARK_LOG_ERR, "SIGCHLD sigaction error\n");
		return -1;
	}

	if(sigaction(SIGINT, &st_act, NULL) != 0) {
		sharkLog(SHARK_LOG_ERR, "SIGINT sigaction error\n");
		return -1;
	}

	if(sigaction(SIGTERM, &st_act, NULL) != 0) {
		sharkLog(SHARK_LOG_ERR, "SIGITERM sigaction error\n");
		return -1;
	}

	sharkLog(SHARK_LOG_DEBUG, "Signal process init successfully\n");
	return 0;
}

int daemonInit() {
	pid_t i_pid1 = 0;
	pid_t i_pid2 = 0;
	int i_fd = 0;
	struct rlimit st_limit =  {0};

	umask(0);

	if(getrlimit(RLIMIT_NOFILE, &st_limit) < 0) {
		sharkLog(SHARK_LOG_ERR, "getrlimit error\n");
	}
	i_pid1 = fork();
	if(i_pid1 < 0) {
		sharkLog(SHARK_LOG_ERR, "child1 process fork failed error\n");
		return -1;
	}
	else if(i_pid1 > 0) {
		/*The first parent exit*/
		exit(0);
	}

	/*ensure the first process exit firstly*/
	sleep(1);

	setsid();
	signalInit();

	i_pid2 = fork();
	if(i_pid2 < 0) {
		sharkLog(SHARK_LOG_ERR, "child process2 fork failed\n");
		return -1;
	}
	else if(i_pid2 > 0) {
		exit(0);
	}

	chdir(SharkdParentDir);

	if(st_limit.rlim_max == RLIM_INFINITY) {
		st_limit.rlim_max = 1024;
	}
	for(i_fd = 0; i_fd < (int)st_limit.rlim_max; i_fd++) {
		close(i_fd);
	}

	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

	sharkdPidUpdate();

	sharkLog(SHARK_LOG_INFO, "Daemon Init successfully\n");
	return 0;
}
