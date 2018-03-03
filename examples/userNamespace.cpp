#include <unistd.h>
#include <sys/types.h>

#include <iostream>

#include "common.hpp"

int main(int argc, char *argv[]){

	char *pidFilePath = (char *)"/home/luguanglong/test/shark_test.txt";
	logOutput(pidFilePath, "uid:%d\n", getuid());
	logOutput(pidFilePath, "gid:%d\n", getgid());

	std::cout << "test_pid_namespace finished\n";
	return 0;
}
