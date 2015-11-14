/*************************************************************************
    > File Name: DemoSecond.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: 2015年11月14日 星期六 21时22分35秒
 ************************************************************************/
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char **argv)
{
	pid_t my_pid;
	int status;
	if((my_pid = fork()) == 0)
	{
		cout << "In child process...\n";
		sleep(3);	//子进程睡眠3秒，看看父进程行为
		cout << "Child pid = " << getpid() << endl;
		cout << "Child's parent pid = " << getppid() << endl;
		exit(EXIT_SUCCESS);
	}
	else if(my_pid > 0)
	{
		waitpid(my_pid, &status, 0);
		cout << "In parent process...\n";
		cout << "Parent pid = " << getpid() << endl;
		cout << "Parent's parent pid = " << getppid() << endl;
		cout << "CHild process exited with status = " << status << endl;
	}
	else
	{
		cout << "Failed to fork...\n";
		exit(EXIT_FAILURE);
	}
	return 0;
}
