/*************************************************************************
    > File Name: udp_socket_server.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Thu Apr  7 16:01:40 2016
 ************************************************************************/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  //INADDR_ANY's head file
#include <unistd.h>  //close's head file
#include <memory.h>
#include <cstdlib>
#include <arpa/inet.h>  //inet_ntoa's head file

const unsigned short int c_port = 1234;
const int c_max_data_size = 1024;

int main(int argc, char** argv)
{
	int sockfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	socklen_t sin_size;
	int num;
	char msg[c_max_data_size];

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		std::cerr << "Failed to create socket." << std::endl;
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(c_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	if (bind(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1)
	{
		std::cerr << "Failed to bind." << std::endl;
		exit(1);
	}

	sin_size = sizeof(struct sockaddr);

	while(true)
	{
		num = recvfrom(sockfd, msg, c_max_data_size, 0, (struct sockaddr*)&client, &sin_size);

		if (num < 0)
		{
			std::cerr << "Failed to recvfrom." << std::endl;
			close(sockfd);
			exit(1);
		}
		msg[num] = '\0';

		std::cout << "You got a message(" << msg << ") from " << inet_ntoa(client.sin_addr) << std::endl;
		sendto(sockfd, "Welcome to my server.\n", 22, 0, (struct sockaddr*)&client, sin_size);

		if (strcmp(msg, "quit") == 0)
		{
			break;
		}

	}
	close(sockfd);
	return 0;
}
