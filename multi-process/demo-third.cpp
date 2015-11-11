/*************************************************************************
 > File Name: DemoThird.cpp
# Author: Jie Mo
# mail: 582865471@vip.qq.com
# Github: JieTrancender
# Created Time: 2015年11月11日 星期三 23时32分32秒
 ************************************************************************/
#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv)
{
	pid_t my_pid;
	int status, i;
	//如果fork() == 0,可以用getid()函数获得该孩子进程的ID，可以用getppid()函数获得父进程的ID
	if((my_pid = fork()) == 0)
	{
		cout << "This is the child process, pid = " << getpid() << ", and my parent's pid = " << getppid() << endl;
		exit(0);
	}
	else if(my_pid > 0)
	{
		sleep(1);
		cout << "This is the parent process, wait for child....\n";
		my_pid = wait(&status);
		i = WEXITSTATUS(status);
		cout << "child's pid = " << my_pid << ", and exit status = " << i << endl;
	}
	return 0;
}
