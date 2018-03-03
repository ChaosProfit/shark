#include <sys/stat.h>
#include <sys/types.h>

#include <iostream>

#include "netNamespace.hpp"
#include "common.hpp"

const static char *RESULT_FILE = "/run/shark/test/netNamespace.log";

int main(int argc, char *argv[]){
	mkdir("/run/shark/test", 0777);
	logOutput(RESULT_FILE, "start\n");

	NetNamepaceTest *tester = new NetNamepaceTest();

	logOutput(RESULT_FILE, "finish\n");

	std::cout << "netNamespace finished\n";
	return 0;
}
