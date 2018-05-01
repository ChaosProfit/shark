/*
 * container.cpp
 *
 *  Created on: Jan 21, 2018
 *      Author: luguanglong
 */
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <limits.h>

#include <list>

#include "container.hpp"
#include "namespaces.hpp"
#include "utils/exceptions.hpp"
#include "utils/log.hpp"
#include "process/process.hpp"

int logFile(const char *data) {
	int fd = open("/tmp/test.log", O_CREAT|O_APPEND|O_RDWR, 0777);
	write(fd, "output:", strlen("output:"));
	write(fd, data, strlen(data));
	write(fd, "\n", strlen("\n"));
	close(fd);

	return 0;
}

void signalProcessFunc(int sig) {

	switch(sig) {
		case SIGPIPE:
			sharkLog(SHARK_LOG_DEBUG, "catch a SIGPIPE signal\n");

			break;
		case SIGTERM:
			sharkLog(SHARK_LOG_DEBUG, "catch a SIGTERM signal\n");

			exit(0);
			break;
		case SIGINT:
			sharkLog(SHARK_LOG_DEBUG, "catch a SIGINT signal\n");

			break;
		case SIGCHLD:
			sharkLog(SHARK_LOG_DEBUG, "catch a SIGCHLD signal\n");

			wait(NULL);
			break;
		default:
			sharkLog(SHARK_LOG_DEBUG, "catch a signal:%d, unprocessed\n", sig);

			break;
	}

	return;
}

int initSignalProcess() {
	struct sigaction st_act =  {0};
	st_act.sa_flags = 0;
	st_act.sa_handler = signalProcessFunc;

	if(sigemptyset(&st_act.sa_mask) != 0) {
		sharkLog(SHARK_LOG_ERR, "sigemptyset error\n");
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
		sharkLog(SHARK_LOG_ERR, "SIGTERM sigaction error\n");
		return -1;
	}

	signal(SIGCHLD, SIG_IGN);
	return 0;
}

int manageProcessFunc(void *args) {
	shark::Container *container = (shark::Container *)args;
	int ret = 0;
	int readPipe = container->getReadPipe();
	char buf[PIPE_BUF] =  {0};

	ret = read(readPipe, buf, PIPE_BUF);
	if((unsigned int)ret != container->getId().size()) {
		sharkLog(SHARK_LOG_DEBUG, "managerProcessFunc create failed, ret:%d, rcv:%s\n", ret , buf);
		return -1;
	}

	utsNsInit(container->getId().data());

	ret = initSignalProcess();

	sharkLog(SHARK_LOG_DEBUG, "manage process,uid:%d,gid:%d, pid:%d\n", getuid(), getgid(), getpid());

	while(1) {

		memset(buf, 0, PIPE_BUF);
		ret = read(readPipe, buf, PIPE_BUF);

		sharkLog(SHARK_LOG_DEBUG, "start exec:%s\n", buf);

		shark::Process *process = new shark::Process(DEFAULT_STACK_SIZE, buf);
		container->addProcess(process);
		process->exec();
		sharkLog(SHARK_LOG_DEBUG, "Process %s create finished\n", buf);

		sleep(1);
		continue;
	}

	return ret;
}

int shark::Container::idInit() {

	id = cCfg.id;

	sharkLog(SHARK_LOG_DEBUG, "Id init finished, ID:%s\n", id.data());
	return 0;
}

int shark::Container::dftCfgInit() {

	if(cCfg.net.addr.str.size() == 0) {
		char tmp[32] =  {0};
		snprintf(tmp, 32, "%u.%u.%u.%u/%u", sCfg.net.bridge.addr.value.array[0],
												 sCfg.net.bridge.addr.value.array[1],
												 sCfg.net.bridge.addr.value.array[2],
												 sCfg.net.bridge.addr.value.array[3] + 1,
												 sCfg.net.bridge.addr.mask);
		cCfg.net.addr.str = tmp;
	}

	sharkLog(SHARK_LOG_DEBUG, "dftCfgInit successfully, data:%s\n", cCfg.net.addr.str.data());
	return 0;
}

shark::Container::Container(ContainerConfig &cCfgArg, SharkConfig &sCfgArg, Cgroup &cgrp):
cCfg(cCfgArg), sCfg(sCfgArg), cgroup(cgrp) {
	int ret = 0;

	ret = dftCfgInit();

	ret = idInit();

	ret = pipe(manageProcessPipe);
	if(ret < 0) {
		sharkLog(SHARK_LOG_DEBUG, "Container construct failed\n");
	}

	cNetwork = new ContainerNetwork(cCfg.id, sCfg.net, cCfg.net);
	sharkLog(SHARK_LOG_DEBUG, "Container construct successfully, readPipe:%d, writePipe:%d\n", manageProcessPipe[0], manageProcessPipe[1]);
	return;
}

shark::Container::~Container() {
	delete manageProcess;

	delete cNetwork;

	sharkLog(SHARK_LOG_DEBUG, "Container destruct successfully\n");
	return;
}

int shark::Container::start() {
	int ret = 0;
	int cloneFlag = CLONE_NEWPID | CLONE_NEWUSER | CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWIPC;

	manageProcess = new Process(DEFAULT_STACK_SIZE, manageProcessFunc, (void *)this, cloneFlag);
	manageProcess->exec();

	userNsInit(manageProcess->getPid());
	ret = write(manageProcessPipe[1], id.data(), id.size());

	sharkLog(SHARK_LOG_DEBUG, "Container %s start successfully, ret:%d\n",\
			 id.data() ,ret);
	return ret;
}

int shark::Container::cmdSend(std::string &execCmd) {
	unsigned int ret = 0;

	ret = write(manageProcessPipe[1], execCmd.data(), execCmd.size());
	if(ret != execCmd.size()) {
		return -1;
	}

	sharkLog(SHARK_LOG_DEBUG, "container exec finished\n");
	return ret;
}

int shark::Container::addProcess(Process *p) {
	processList.push_back(p);

	cgroup.addTask(id, p->getPid());
	sharkLog(SHARK_LOG_DEBUG, "container %s addProcess %d successfully\n", id.data(), p->getPid());
	return 0;
}

