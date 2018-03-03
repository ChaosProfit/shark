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
		Process(int stackSize, int (*execFunc)(void *args), void *argv, int cloneFlags);
		~Process();

		int exec();

		void *stackTop(){
			return (void *)((char *)stackPtr + stackSize);
		};

		int getPid(){
			return this->pid;
		};

		std::string &getExecCmd(){
			return execCmd;
		}

	private:
		std::string execCmd;

		int execFunction();
		int (*funcExecCallback)(void *args) = NULL;

		int stackSize;
		int cloneFlags;
		void *stackPtr;
		int pid = 0;
		void *execFuncArg = NULL;
		std::list<Process *> childProcessList;
	};
}


#endif /* LIBCONTAINER_CONTAINER_PROCESS_HPP_ */
