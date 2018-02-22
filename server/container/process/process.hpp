/*
 * process.hpp
 *
 *  Created on: Jan 23, 2018
 *      Author: luguanglong
 */

#ifndef LIBCONTAINER_CONTAINER_PROCESS_HPP_
#define LIBCONTAINER_CONTAINER_PROCESS_HPP_

#include <string>
#include <list>

#include <unistd.h>

#define DEFAULT_STACK_SIZE  (1024*1024)

namespace shark {
	typedef enum {
		EXEC_BIN = 0,
		EXEC_FUNC = 1,
	} EXEC_TYPE;

	int execFunc(void *args);

	class Process{
	public:
		Process(int stackSize, const char *execCmd);
		Process(int stackSize, int (*execFunc)(void *args));
		Process(int stackSize, int (*execFunc)(void *args), void *argv, int cloneFlags);
		~Process();

		int exec();

		void *stackTop(){
			return (void *)((char *)stackPtr + stackSize);
		};

		int getPid(){
			return this->pid;
		};

		char **getArgv(){
			return argv;
		};

		char *getExecBin(){
			return argv[0];
		};
	private:
		int cmdCheck();
		int execFunction();
		int (*funcExecCallback)(void *args) = NULL;

		int stackSize;
		int cloneFlags;
		int argvGenerate(std::string &execCmd);
		void *stackPtr;
		int pid = 0;
		char **argv = NULL;
		void *execFuncArg = NULL;
		unsigned int argNum = 0;
		EXEC_TYPE type = EXEC_BIN;
		std::list<Process *> childProcessList;
	};
}


#endif /* LIBCONTAINER_CONTAINER_PROCESS_HPP_ */
