/*************************************************************************
    > File Name: tcp_socket.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Thu Apr  7 12:59:41 2016
 ************************************************************************/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  //INADDR_ANY's head file
#include <unistd.h>  //close's head file
#include <memory.h>
#include <cstdlib>
#include <cstdio>
#include <arpa/inet.h>  //inet_ntoa's head file
#include <cstring>

const unsigned short int c_port = 1234;
const int c_max_connection = 4;

int main(int argc, char **argv)
{
	struct sockaddr_in client;
	struct sockaddr_in server;
	int listenfd, connectfd;

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

	if ((connectfd = accept(listenfd, (struct sockaddr*)&client, &sin_size)) == -1)
	{
		std::cerr << "Failed to accept." << std::endl;
		exit(1);
	}

	std::cout << "You got a connection from " << inet_ntoa(client.sin_addr) << std::endl;
	
	send(connectfd, "welcome to my server.", 22, 0);
	close(connectfd);
	close(listenfd);
	return 0;
}
