/************************************************************************
 > File Name: server.cpp
# Author: Jie Mo
# mail: 582865471@vip.qq.com
# Github: JieTrancender
# Created Time: 2015年11月10日 星期二 10时14分07秒
 ************************************************************************/
#include <iostream>
#include <string>
#include <algorithm>
#include <memory.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

const int c_port = 1234;
const int c_back_log = 5;
const int c_data_size = 1024;

//声明“客户处理”process_cli()函数，用于处理客户请求
void process_cli(int connectfd, sockaddr_in client);

//声明线程所执行的函数
void* start_routine(void *arg);

//定义ARG结构，用于主线程传递参数给新线程
struct ARG
{
	int connectfd;
	sockaddr_in client;
};

int main(int argc, char **argv)
{
	int listenfd, connectfd;
	pthread_t my_thread;
	ARG *arg;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t sin_size;

	//创建TCP套接字
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		cout << "Failed to create socket ." << endl;
		return 0;
	}

	//设置套接字选项为SO_REUSEADDR
	int opt = SO_REUSEADDR;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	//绑定套接字到相应地址，INADDR_ANY宏表示可接收来自本机的任何IP地址的客户连接
	memset((void *)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(c_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(listenfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{
		cout << "Failed to bind ." << endl;
		return 0;
	}

	//监听网络连接
	if(listen(listenfd, c_back_log) == -1)
	{
		cout << "Failed to listen ." << endl;
		return 0;
	}

	sin_size = sizeof(struct sockaddr_in);

	//接收客户连接，一旦连接，产生新线程服务客户，然后关闭连接套接字并继续接收下一客户连接
	while(true)
	{
		if((connectfd = accept(listenfd, (struct sockaddr *)&client, &sin_size)) == -1)
		{
			cout << "Failed to accept ." << endl;
			return 0;
		}

		//分配空间给arg，然后把连接套接字和客户地址信息赋值给arg
		arg = new ARG;
		arg->connectfd = connectfd;
		memcpy((void *)&arg->client, &client, sizeof(client));
		
		//产生新线程
		if(pthread_create(&my_thread, nullptr, start_routine, (void *)arg))
		{
			cout << "Failed to create new thread ." << endl;
			return 0;
		}
	}
	close(listenfd);
	return 0;
}

//实现服务功能
void process_cli(int connectfd, sockaddr_in client)
{
	int num;
	char recvbuf[c_data_size], sendbuf[c_data_size], cli_name[c_data_size];
	cout << "You got a connection from: " << inet_ntoa(client.sin_addr) << ".\n";

	//从客户端获得客户的名字
	num = recv(connectfd, cli_name, c_data_size, 0);
	if(0 == num)
	{
		close(connectfd);
		cout << "Failed to connect client ." << endl;
		return;
	}
	cout << "Client's name is: " << cli_name << endl;
	while(num = recv(connectfd, recvbuf, c_data_size, 0))
	{
		cout << "Received client(" << cli_name << ") message: " << recvbuf << endl;
		reverse(recvbuf, recvbuf + strlen(recvbuf));
		strcpy(sendbuf, recvbuf);
		send(connectfd, sendbuf, strlen(sendbuf), 0);
	}
	close(connectfd);
	return;
}

//实现start_routine()函数
void* start_routine(void *arg)
{
	ARG *info;
	info = (ARG *)arg;

	//处理客户需求
	process_cli(info->connectfd, info->client);
	delete info;
	pthread_exit(nullptr);
}
