/*************************************************************************
 > File Name: DemoSecond.cpp
# Author: Jie Mo
# mail: 582865471@vip.qq.com
# Github: JieTrancender
# Created Time: 2015年11月11日 星期三 22时37分17秒
 ************************************************************************/
#include <iostream>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

int main()
{
	pid_t my_pid;
	int status;
	if((my_pid = vfork()) == 0)
	{
		sleep(2);
		cout << "Child running.\n";
		cout << "Child sleeping.\n";
		sleep(2);
		cout << "Child dead.\n";
		exit(0);
	}
	else if(my_pid > 0)
	{
		cout << "Parent running.\n";
		cout << "Parent exit.\n";
		exit(0);
	}
	else
	{
		cout << "Failed to vfork.\n";
		exit(1);
	}
	return 0;
}
