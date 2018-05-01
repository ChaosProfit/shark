/*
 * process.cpp
 *
 *  Created on: Jan 23, 2018
 *      Author: luguanglong
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

#include <string>

#include "process.hpp"
#include "utils/exceptions.hpp"
#include "utils/log.hpp"

int binExecCallback(void *args) {
	int ret = 0;
	shark::Process *process = (shark::Process *)args;
	std::string &cmd = process->getExecCmd();

	sharkLog(SHARK_LOG_DEBUG, "uid:%d, gid:%d\n", getuid(), getgid());
	sharkLog(SHARK_LOG_DEBUG, "cmd:%s", cmd.data());

	ret = execlp("bash", "bash", "-c", cmd.data(), nullptr);

	sharkLog(SHARK_LOG_DEBUG, "ret:%d, err:%d, cmd:%s\n", ret, errno, cmd.data());
	return ret;
}

shark::Process::Process(int stackSize, const char *cmd):\
funcExecCallback(binExecCallback), stackSize(stackSize), cloneFlags(0) {
	execCmd = cmd;
	execFuncArg = this;
	stackPtr = malloc(stackSize);
	memset(stackPtr, 0, stackSize);

	sharkLog(SHARK_LOG_DEBUG, "Process construct successfully, cmd:%s\n", execCmd.data());
}

shark::Process::Process(int stackSize, int (*execFunc)(void *args), \
						void *objectPtr, int cloneFlags):\
funcExecCallback(execFunc), stackSize(stackSize), cloneFlags(cloneFlags) {
	stackPtr = malloc(stackSize);
	memset(stackPtr, 0, stackSize);
	execFuncArg = objectPtr;

	sharkLog(SHARK_LOG_DEBUG, "Process3 construct successfully\n");
}

shark::Process::~Process() {
	int ret = 0;

	free(stackPtr);

	for(auto item = childProcessList.begin(); item != childProcessList.end(); item++) {
		delete *item;
	}

	kill(pid, SIGTERM);
	siginfo_t info =  {0};
	ret = waitid(P_PID, pid, &info, WEXITED|WSTOPPED|__WCLONE);

	sharkLog(SHARK_LOG_DEBUG, "process %d destruct successfully, ret:%d, err:%d\n", pid, ret, errno);
	return;
}

int shark::Process::exec() {
	int ret = 0;

   if (funcExecCallback == nullptr) {
	   sharkLog(SHARK_LOG_ERR, "process exec failed\n");
	   return -1;
    }

   ret = clone(funcExecCallback, stackTop(), cloneFlags, execFuncArg);
   if (ret < 0) {
		sharkLog(SHARK_LOG_ERR, "child process created failed\n");
		throw new SharkException("child process created failed");
	}

	pid = ret;

	sharkLog(SHARK_LOG_DEBUG, "child process %d created successfully\n", pid);
	return 0;
}
