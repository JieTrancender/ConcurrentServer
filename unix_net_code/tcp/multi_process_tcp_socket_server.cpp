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
	sersockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	server.sin_family = AF_INET;
	server.sin_port = htons(c_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	if (bind(listenfd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
	{
		std::cerr << "Failed to bind." << std::endl;
		close(listenfd);
		exit(1);
	}

	if (listen(listenfd, c_max_connection) == -1)
	{
		std::cerr << "Failed to listen." << std::endl;
		close(listenfd);
		exit(1);
	}

	auto sin_size = sizeof(struct  sockaddr_in);
	while(true)
	{
		if (connectfd = accept(listenfd, (struct sockaddr*)&client, &sin_size) == -1)
		{
			std::cout << "Failed to accept." << std::endl;
			close(listenfd);
			exit(1);
		}


	}
}
