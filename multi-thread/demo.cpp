/*************************************************************************
 > File Name: demo.cpp
# Author: Jie Mo
# mail: 582865471@vip.qq.com
# Github: JieTrancender
# Created Time: 2015年11月10日 星期二 10时57分13秒
 ************************************************************************/
#include <iostream>
#include <pthread.h>

using namespace std;

//线程函数，打印n次“Welcome to new thread .”
void *print(void *n)
{
	int count = *(int *)n;
	while(count--)
	{
		cout << ": Welcome to new thread .\n";
	}
	return nullptr;
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		cout << "Argument error !\n";
		return 0;
	}

	pthread_t my_thread;
	int n = *argv[1] - '0';
	if(pthread_create(&my_thread, nullptr, print, (void *)&n))
	{
		cout << "Failed to create thread ." << endl;
		return 0;
	}

	if(pthread_join(my_thread, nullptr))
	{
		cout << "Failed to join thread .\n";
		return 0;
	}
	return 0;
}
