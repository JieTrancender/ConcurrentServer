/*************************************************************************
    > File Name: multi_thread_tcp_socket_server.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Tue Apr 12 14:39:24 2016
 ************************************************************************/
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <cstring>
#include <netinet/in.h>  //INADDR_ANY's header
#include<arpa/inet.h>  //inet_ntoa's header

const unsigned short int c_port = 1234;
const int c_max_connection = 4;
const int c_max_data_size = 1024;

struct Arg
{
	int connectfd;
	struct sockaddr_in client;
};

void* start_routine(void *arg);
void process_client(int connectfd, struct sockaddr_in client);

int main(int argc, char** argv)
{
	int listenfd, connectfd;
	struct sockaddr_in server, client;
	Arg *arg;
	pthread_t thread;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cerr << "Failed to create socket." << std::endl;
		exit(1);
	}

	int opt = SO_REUSEADDR;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	server.sin_family = AF_INET;
	server.sin_port = htons(c_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	if (bind(listenfd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
	{
		std::cerr << "Failed to bind." << std::endl;
		exit(1);
	}

	if (listen(listenfd, c_max_connection) == -1)
	{
		std::cerr << "Failed to listen." << std::endl;
		exit(1);
	}

	socklen_t sin_size = sizeof(struct sockaddr_in);
	while(true)
	{
		if ((connectfd = accept(listenfd, (struct sockaddr*)&client, &sin_size)) == -1)
		{
			std::cerr << "Failed to accept." << std::endl;
			close(listenfd);
			exit(1);
		}
		
		arg = new Arg;
		arg->connectfd = connectfd;
		memcpy((void*)&arg->client, &client, sizeof(client));

		if (pthread_create(&thread, nullptr, start_routine, (void*)arg))
		{
			std::cerr << "Failed to create thread." << std::endl;
			close(listenfd);
			exit(1);
		}
	}
	close(listenfd);
	return 0;
}

void* start_routine(void *arg)
{
	Arg *info;
	info = (Arg*)arg;

	process_client(info->connectfd, info->client);
	delete info;
	pthread_exit(nullptr);
}

void process_client(int connectfd, struct sockaddr_in client)
{
	int num;
	char recvbuf[c_max_data_size], sendbuf[c_max_data_size], client_name[c_max_data_size];

	std::cout << "You got a connection from " << inet_ntoa(client.sin_addr);

	if ((num = recv(connectfd, client_name, c_max_data_size, 0)) == 0)
	{
		std::cerr << "Wrong, client disconnected." << std::endl;
		close(connectfd);
		return;
	}
	client_name[num - 1] = '\0';
	std::cout << ", client's name is " << client_name << std::endl;

	while (num = recv(connectfd, recvbuf, c_max_data_size, 0))
	{
		recvbuf[num] = '\0';
		std::cout << "Received client(" << client_name << ") message:" << recvbuf;

		for (int i = 0; i < num - 1; ++i)
		{
			sendbuf[i] = recvbuf[num - i - 2];
		}
		sendbuf[num - 1] = '\0';
		send(connectfd, sendbuf, strlen(sendbuf), 0);
	}
	close(connectfd);
}
