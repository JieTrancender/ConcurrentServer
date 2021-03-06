/*************************************************************************
    > File Name: multi_process_tcp_socket_server.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Mon Apr 11 18:51:16 2016
 ************************************************************************/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  //INADDR_ANY's header
#include <unistd.h>  //close's header
#include <memory.h>
#include <cstdlib>
#include <arpa/inet.h>  //inet_ntoa's header
#include <cstring>

const unsigned short int c_port = 1234;
const int c_max_connection = 4;
const int c_max_data_size = 1024;

void process_client(int connectfd, struct sockaddr_in client);

int main(int argc, char** argv)
{
	struct sockaddr_in server, client;
	int listenfd, connectfd;
	pid_t pid;

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

	socklen_t sin_size = sizeof(struct  sockaddr_in);
	while(true)
	{
		if ((connectfd = accept(listenfd, (struct sockaddr*)&client, &sin_size)) == -1)
		{
			std::cout << "Failed to accept." << std::endl;
			close(listenfd);
			exit(1);
		}

		if (pid = fork() > 0)
		{
			close(connectfd);
			continue;
		}
		else if (pid == 0)
		{
			close(listenfd);
			process_client(connectfd, client);
		}
		else
		{
			std::cerr << "Failed to fork." << std::endl;
			close(listenfd);
			exit(1);
		}
	}
	close(listenfd);
	return 0;
}

void process_client(int connectfd, struct sockaddr_in client)
{
	int num_byte;
	char recvbuf[c_max_data_size], sendbuf[c_max_data_size], client_name[c_max_data_size];

	std::cout << "You got a connection from " << inet_ntoa(client.sin_addr);
	num_byte = recv(connectfd, client_name, c_max_data_size, 0);
	if (num_byte == 0)
	{
		std::cerr << "Failed to connecte client." << std::endl;
		close(connectfd);
		exit(1);
	}
	client_name[num_byte - 1] = '\0';

	std::cout << ", Client's name is " << client_name << std::endl;

	while (num_byte = recv(connectfd, recvbuf, c_max_data_size, 0))
	{
		recvbuf[num_byte] = '\0';
		
		std::cout << "Received client(" << client_name << ") message:" << recvbuf;

		for (int i = 0; i < num_byte - 1; ++i)
		{
			sendbuf[i] = recvbuf[num_byte - i - 2];
		}
		sendbuf[num_byte - 1] = '\0';

		send(connectfd, sendbuf, strlen(sendbuf), 0);
	}
	close(connectfd);
}
