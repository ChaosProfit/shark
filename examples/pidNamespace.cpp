#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <iostream>

int main(int argc, char *argv[]){
	int fd = 0;
	int ret = 0;
	int pid = 0;
	char pidBuf[36] = {0};

//	char *pidFilePath = argv[1];
	mkdir("/run/shark/luguanglong", 0777);
	char *pidFilePath = (char *)"/run/shark/test/pid_namespace.test";
	fd = open(pidFilePath, O_CREAT|O_RDWR|O_APPEND);
	if(fd < 0){
		std::cout << "Open file Failed\n";
		return -1;	
	}
	
	pid = getpid();
	snprintf(pidBuf, 36, "%d", pid);

	ret = write(fd, pidBuf, strlen(pidBuf));
	if(ret < 0){
		std::cout << "Write " << pidFilePath << " failed\n";
		return -1;
	}

	close(fd);

	std::cout << "test_pid_namespace finished\n";
	return 0;
}
