/*
 * misc.cpp
 *
 *  Created on: 2018年2月10日
 *      Author: luguanglong
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <regex>

#include "misc.hpp"
#include "utils/log.hpp"

static const char *SharkdParentDir = "/run/shark";

int sharkdPidUpdate(){
	int pidFd = 0;
	int ret = 0;
	std::string pidPath(SharkdParentDir);
	char pidStr[36] = {0};
	snprintf(pidStr, 36, "%d", getpid());

	pidPath.append("/");
	pidPath.append("sharkd.pid");

	pidFd = open(pidPath.data(), O_CREAT|O_RDWR);
	if(pidFd < 0){
		sharkLog(SHARK_LOG_ERR, "%s open failed\n", pidPath.data());
		return -1;
	}

	ret = write(pidFd, pidStr, strlen(pidStr));
	if(ret != (signed int)strlen(pidStr)){
		sharkLog(SHARK_LOG_ERR, "%s write failed\n", pidPath.data());
		close(pidFd);
		return -1;
	}

	sharkLog(SHARK_LOG_ERR, "%s update successfully, pid:%s\n", pidPath.data(), pidStr);
	close(pidFd);
	return 0;
}

int rmDir(const char *dirPath)
{
	int ret = 0;
	DIR* dirp = opendir(dirPath);
	if(!dirp){
		return -1;
	}

	struct dirent *dir;
	struct stat st;
	while((dir = readdir(dirp)) != NULL){
		if((strcmp(dir->d_name,".") == 0) || (strcmp(dir->d_name,"..") == 0)){
			continue;
		}

		std::string sub_path = std::string(dirPath) + '/' + dir->d_name;
		if(lstat(sub_path.c_str(),&st) == -1){
			continue;
		}

		if(S_ISDIR(st.st_mode)){
			// 如果是目录文件，递归删除
			if(rmDir(sub_path.data()) == -1){
				closedir(dirp);
				return -1;
			}

			rmdir(sub_path.c_str());
		}
		else{
			ret = unlink(sub_path.c_str());     // 如果是普通文件，则unlink
		}
	}

	if(rmdir(dirPath) == -1){
		closedir(dirp);
		return -1;
	}

	closedir(dirp);
	return ret;
}

int cmdExecSync(const char *format ,...)
{
	char argBuf[1024] = {0};
	char retBuf[256] = {0};

	va_list argLst;
	va_start(argLst,format);
	vsnprintf(argBuf, 1024, format, argLst);
	va_end(argLst);

	FILE *p_file = popen(argBuf, "r");
	if(p_file == NULL){
		sharkLog(SHARK_LOG_ERR, "%s exec failed\n", argBuf);
		return -1;
	}
	while(fgets(retBuf, 256, p_file) != NULL) {
		sharkLog(SHARK_LOG_DEBUG, "popen_ret:%s\n", retBuf);
	}
	pclose(p_file);

	sharkLog(SHARK_LOG_ERR, "cmdExecSync successfully\n");
	return 0;
}


int dirInit(){
	DIR *pd_var_dir = NULL;

	pd_var_dir = opendir(SharkdParentDir);
	if(pd_var_dir == NULL){
		if(mkdir(SharkdParentDir, 0777) < 0){
			sharkLog(SHARK_LOG_ERR, "mkdir %s error\n", SharkdParentDir);
			return -1;
		}
	}
	else{
		closedir(pd_var_dir);
	}
	return 0;
}
