/*************************************************************************
 > File Name: DemoFirst.cpp
# Author: Jie Mo
# mail: 582865471@vip.qq.com
# Github: JieTrancender
# Created Time: 2015年11月11日 星期三 21时36分45秒
 ************************************************************************/
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main()
{
	pid_t my_pid;
	int status;
	//如果创建的进程号等于0,则先休眠2秒，再打印出有关的信息
	if((my_pid = fork()) == 0)	//子进程返回0
	{
		sleep(2);
		cout << "Child running.\n";
		cout << "Child sleeping.\n";
		sleep(2);
		cout << "Child dead.\n";
		exit(0);
	}

	//如果创建的进程号大于0,则显示出父进程的有关信息
	else if(my_pid > 0)
	{
		cout << "Parent running.\n";
		cout << "Parent exit.\n";
		exit(0);
	}
	else
	{
		cout << "Failed to fork.\n";
		exit(1);
	}
	return 0;
}
