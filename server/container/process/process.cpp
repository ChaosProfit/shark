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

int binExecCallback(void *args){
	int ret = 0;
	shark::Process *process = (shark::Process *)args;
	sharkLog(SHARK_LOG_DEBUG, "arg1:%s,arg2:%s\n", process->getExecBin(), process->getArgv()[0]);

	ret = execvp(process->getExecBin(), process->getArgv());

	sharkLog(SHARK_LOG_DEBUG, "ret:%d, err:%d\n", ret, errno);
	return ret;
}

shark::Process::Process(int stackSize, const char *execCmd):\
funcExecCallback(binExecCallback), stackSize(stackSize), cloneFlags(0){
	std::string execCmdStr(execCmd);

	argvGenerate(execCmdStr);
	execFuncArg = this;

	stackPtr = malloc(stackSize);
	memset(stackPtr, 0, stackSize);

	sharkLog(SHARK_LOG_DEBUG, "Process1 construct successfully\n");
}

shark::Process::Process(int stackSize, int (*execFunc)(void *args)):\
funcExecCallback(execFunc), stackSize(stackSize), cloneFlags(0){

	stackPtr = malloc(stackSize);
	memset(stackPtr, 0, stackSize);
	if(funcExecCallback != NULL){
		type = EXEC_FUNC;
	}

	sharkLog(SHARK_LOG_DEBUG, "Process2 construct successfully\n");
}

shark::Process::Process(int stackSize, int (*execFunc)(void *args), void *objectPtr, int cloneFlags):\
funcExecCallback(execFunc), stackSize(stackSize), cloneFlags(cloneFlags){

	stackPtr = malloc(stackSize);
	memset(stackPtr, 0, stackSize);
	execFuncArg = objectPtr;

	sharkLog(SHARK_LOG_DEBUG, "Process3 construct successfully\n");
}

shark::Process::~Process(){
	unsigned int index = 0;
	int status = 0;
	int ret = 0;

	if(argv != NULL){
		for(index = 0; index < argNum; index++){
			free(argv[index]);
		}

		free(argv);
	}
	free(stackPtr);

	for(auto item = childProcessList.begin(); item != childProcessList.end(); item++){
		delete *item;
	}

	kill(pid, SIGTERM);
	siginfo_t info = {0};
	ret = waitid(P_PID, pid, &info, WEXITED|WSTOPPED|__WCLONE);

	sharkLog(SHARK_LOG_DEBUG, "process %d destruct successfully, ret:%d, err:%d\n", pid, ret, errno);
	return;
}

int shark::Process::cmdCheck() {
	int ret = 0;
	struct stat state = {0};

	ret = stat(getExecBin(), &state);
	if(ret < 0){
		sharkLog(SHARK_LOG_ERR, "%s does not existed\n", getExecBin());
		throw new SharkException("bin does not eisted");
	}

	if(!S_ISREG(state.st_mode) && !S_ISLNK(state.st_mode)){
		sharkLog(SHARK_LOG_ERR, "%s is not a file\n", getExecBin());
		throw new SharkException("bin is not a file");
	}

	if((state.st_mode & S_IXUSR) != S_IXUSR){
		sharkLog(SHARK_LOG_ERR, "%s can not be executed\n", getExecBin());
		throw new SharkException("can not be executed");
	}

	sharkLog(SHARK_LOG_DEBUG, "command check successfully\n");
	return 0;
}

int shark::Process::exec(){
	int ret = 0;

   if(funcExecCallback == NULL){
	   sharkLog(SHARK_LOG_ERR, "process exec failed\n");
	   return -1;
   }

   ret = clone(funcExecCallback, stackTop(), cloneFlags, execFuncArg);
	if(ret < 0){
		sharkLog(SHARK_LOG_ERR, "child process created failed\n");
		throw new SharkException("child process created failed");
	}

	pid = ret;

	sharkLog(SHARK_LOG_DEBUG, "child process %d created successfully\n", pid);
	return 0;
}


int shark::Process::argvGenerate(std::string &execCmd){
	unsigned int charIndex = 0;
	unsigned int strIndex = 0;
	const char *execCmdPtr = execCmd.c_str();
	unsigned int begin = 0;
	unsigned int end = 0;

	char *tmp = (char *)malloc(execCmd.size() + 1);
	memset(tmp, 0, (execCmd.size() + 1));
	strncpy(tmp, execCmd.c_str(), execCmd.size());
	for(charIndex=0; charIndex < execCmd.size(); charIndex++){
		if(execCmdPtr[charIndex] == ' '){
			argNum++;
		}
	}

	argv = (char **)malloc(argNum+1);
	memset(argv, 0, (argNum+2));

	for(charIndex=0; charIndex < execCmd.size(); charIndex++){
		if(execCmdPtr[charIndex] == ' '){
			end = charIndex;
			argv[strIndex] = (char *)malloc(end-begin+1);
			memset(argv[strIndex], 0, (end-begin+1));
			strncpy(argv[strIndex], &execCmdPtr[begin], (end - begin));
			begin = charIndex + 1;
			strIndex++;
		}
	}

	end = charIndex;
	argv[strIndex] = (char *)malloc(end-begin+1);
	strncpy(argv[strIndex], &execCmdPtr[begin], (end-begin));

	sharkLog(SHARK_LOG_DEBUG, "%s arg generate successfully\n", execCmd.data());
	return 0;
}
