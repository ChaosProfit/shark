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
#include "utils/exceptions.hpp"
#include "utils/log.hpp"
#include "process/process.hpp"

int logFile(const char *data){
	int fd = open("/tmp/test.log", O_CREAT|O_APPEND|O_RDWR, 0777);
	write(fd, "output:", strlen("output:"));
	write(fd, data, strlen(data));
	write(fd, "\n", strlen("\n"));
	close(fd);

	return 0;
}

void signalProcessFunc(int sig){

	switch(sig){
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

int initSignalProcess(){
	struct sigaction st_act = {0};
	st_act.sa_flags = 0;
	st_act.sa_handler = signalProcessFunc;

	if(sigemptyset(&st_act.sa_mask) != 0){
		sharkLog(SHARK_LOG_ERR, "sigemptyset error\n");
		return -1;
	}

	if(sigaction(SIGPIPE, &st_act, NULL) != 0){
		sharkLog(SHARK_LOG_ERR, "SIGPIPE sigaction error\n");
		return -1;
	}

	if(sigaction(SIGCHLD, &st_act, NULL) != 0){
		sharkLog(SHARK_LOG_ERR, "SIGCHLD sigaction error\n");
		return -1;
	}

	if(sigaction(SIGINT, &st_act, NULL) != 0){
		sharkLog(SHARK_LOG_ERR, "SIGINT sigaction error\n");
		return -1;
	}

	if(sigaction(SIGTERM, &st_act, NULL) != 0){
		sharkLog(SHARK_LOG_ERR, "SIGTERM sigaction error\n");
		return -1;
	}

	signal(SIGCHLD, SIG_IGN);
	return 0;
}

int manageProcessFunc(void *args){
	shark::Container *container = (shark::Container *)args;
	int ret = 0;
	int readPipe = container->getReadPipe();
	char buf[PIPE_BUF] = {0};

	ret = initSignalProcess();

	sharkLog(SHARK_LOG_DEBUG, "manage process pid:%d\n", getpid());

	while(1){

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

shark::Container::Container(ContainerConfig &cCfg, SharkConfig &sCfg):cCfg(cCfg), sCfg(sCfg){
	int ret = 0;

	ret = pipe(manageProcessPipe);
	if(ret < 0){
		sharkLog(SHARK_LOG_DEBUG, "Container construct failed\n");
	}

	cNetwork = new ContainerNetwork(cCfg.id, sCfg.net, cCfg.net);
	sharkLog(SHARK_LOG_DEBUG, "Container construct successfully, readPipe:%d, writePipe:%d\n", manageProcessPipe[0], manageProcessPipe[1]);
	return;
}

shark::Container::~Container(){
	delete manageProcess;

	delete cNetwork;

	sharkLog(SHARK_LOG_DEBUG, "Container destruct successfully\n");
	return;
}

int shark::Container::start(){
	int ret = 0;
//	int cloneFlag = CLONE_FS | CLONE_NEWPID | CLONE_NEWNET;

	int cloneFlag = CLONE_FS;

	manageProcess = new Process(DEFAULT_STACK_SIZE, manageProcessFunc, (void *)this, cloneFlag);
	manageProcess->exec();

	sharkLog(SHARK_LOG_DEBUG, "Container start successfully, ret:%d\n", ret);
	return ret;
}

int shark::Container::cmdSend(std::string &execCmd){
	unsigned int ret = 0;

	ret = write(manageProcessPipe[1], execCmd.data(), execCmd.size());
	if(ret != execCmd.size()){
		return -1;
	}

	sharkLog(SHARK_LOG_DEBUG, "container exec finished\n");
	return ret;
}

int shark::Container::addProcess(Process *p){
	processList.push_back(p);

	sharkLog(SHARK_LOG_DEBUG, "addProcess successfully\n");
	return 0;
}
