/*************************************************************************
    > File Name: thread_synchronization.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Thu Apr 14 13:43:35 2016
 ************************************************************************/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

const unsigned short int c_port = 1234;
const int c_max_data_size = 1024;

int main(int argc, char** argv)
{
	int fd, num;
	char buf[c_max_data_size];
	struct hostent *he;
	struct sockaddr_in server, reply;

	if (argc != 3)
	{
		std::cerr << "Usage:<IP Address><message>" << std::endl;
		exit(1);
	}

	if ((he = gethostbyname(argv[1])) == nullptr)
	{
		std::cerr << "Failed to get host." << std::endl;
		exit(1);
	}

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		std::cerr << "Failed to create socket." << std::endl;
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(c_port);
	server.sin_addr = *((struct in_addr*)he->h_addr);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	sendto(fd, argv[2], strlen(argv[2]), 0, (struct sockaddr*)&server, sizeof(struct sockaddr));

	while(true)
	{
		socklen_t len;

		if ((num = recvfrom(fd, buf, c_max_data_size, 0, (struct sockaddr*)&reply, &len)) == -1)
		{
			std::cerr << "Failed to use recvfrom." << std::endl;
			close(fd);
			exit(1);
		}

		if ((int)len != sizeof(struct sockaddr) || memcmp((const void*)&server, (const void*)&reply, len) != 0)
		{
			std::cout << "Received from other server." << std::endl;
			continue;
		}

		buf[num] = '\0';
		std::cout << "Server message:" << buf << std::endl;
		break;
	}
	close(fd);
	return 0;
}
