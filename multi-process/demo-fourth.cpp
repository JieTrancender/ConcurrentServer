/*************************************************************************
    > File Name: DemoFirst.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: 2015年11月14日 星期六 21时15分15秒
 ************************************************************************/
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv)
{
	pid_t my_pid;
	int status;

	//fork()函数 - 子进程返回0，父进程返回子进程ID号
	if((my_pid = fork()) == 0)
	{
		cout << "Child running...\n";
		sleep(1);
		cout << "Child sleeping...\n";
		cout << "Child dead...\n";
		exit(0);
	}
	else if(my_pid > 0)
	{
		cout << "Parent running...\n";
		waitpid(my_pid, &status, 0);
		cout << "Parent is running...\n";
		cout << "Parent exit...\n";
	}
	else
	{
		cout << "Failed to fork...\n";
		exit(1);
	}
	return 0;
}
