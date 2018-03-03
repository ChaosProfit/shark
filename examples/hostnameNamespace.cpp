#include <unistd.h>
#include <sys/types.h>

#include <iostream>

#include "common.hpp"

int main(int argc, char *argv[]){
	char *pidFilePath = (char *)"/home/luguanglong/test/shark_test.txt";
	char tmpBuf[64] = {0};

	int ret = gethostname(tmpBuf, 64);
	if(ret < 0){
		return -1;
	}

	logOutput(pidFilePath, "hostname:%s\n", tmpBuf);

	std::cout << "test_pid_namespace finished\n";
	return 0;
}
