/*************************************************************************
    > File Name: tcp_socket_client.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Thu Apr  7 14:22:50 2016
 ************************************************************************/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <memory.h>
#include <cstdlib>
#include <netdb.h>  //gethostbyname's head file

const unsigned short int c_port = 1234;
const unsigned int c_max_data_size = 1024;

int main(int argc, char **argv)
{
	struct sockaddr_in server;
	int fd, num_byte;
	char buf[c_max_data_size];
	struct hostent *he;

	if (argc != 2)
	{
		std::cerr << "Usage:" << argv[0] << " <IP Address>" << std::endl;
		exit(1);
	}

	if ((he = gethostbyname(argv[1])) == nullptr)
	{
		std::cerr << "Failed to get host." << std::endl;
		exit(1);
	}

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cout << "Failed to socket." << std::endl;
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(c_port);
	server.sin_addr = *((in_addr*)he->h_addr);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));
	
	if (connect(fd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
	{
		std::cout << "Failed to connect." << std::endl;
		exit(1);
	}

	if ((num_byte = recv(fd, buf, c_max_data_size, 0)) == -1)
	{
		std::cout << "Failed to recv." << std::endl;
		close(fd);
		exit(1);
	}

	buf[num_byte] = '\0';
	std::cout << "Server Message is: " << buf << std::endl;

	close(fd);
	return 0;
}
