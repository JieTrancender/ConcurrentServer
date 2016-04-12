/*************************************************************************
    > File Name: multi_process_tcp_socket_client.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Tue Apr 12 13:08:53 2016
 ************************************************************************/
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

const unsigned short int c_port = 1234;
const int c_max_data_size = 1024;

void process(FILE *fp, int sockfd);
char* get_message(char *send_line, int len, FILE *fp);

int main(int argc, char** argv)
{
	int fd;
	struct hostent *he;
	struct sockaddr_in server;

	if (argc != 2)
	{
		std::cerr << "Usage:" << argv[0] << "<IP Address" << std::endl;
		exit(1);
	}

	if ((he = gethostbyname(argv[1])) == nullptr)
	{
		std::cerr << "Failed to get host." << std::endl;
		exit(1);
	}

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cerr << "Failed to create socket." << std::endl;
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(c_port);
	server.sin_addr = *((struct in_addr*)he->h_addr);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	if (connect(fd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
	{
		std::cerr << "Failed to connect." << std::endl;
		perror("Failed to connect.");
		exit(1);
	}

	process(stdin, fd);
	close(fd);
	return 0;
}

void process(FILE *fp, int sockfd)
{
	char send_line[c_max_data_size], recv_line[c_max_data_size];
	int num_byte;

	std::cout << "Succeed to connecte server, please input your name:";
	if (fgets(send_line, c_max_data_size, fp) == nullptr)
	{
		std::cerr << "Failed to input name." << std::endl;
		return;
	}

	send(sockfd, send_line, strlen(send_line), 0);

	while (get_message(send_line, c_max_data_size, fp) != nullptr)
	{
		send(sockfd, send_line, strlen(send_line), 0);

		if ((num_byte = recv(sockfd, recv_line, c_max_data_size, 0)) == -1)
		{
			std::cerr << "Server terminated." << std::endl;
			return;
		}
		recv_line[num_byte] = '\0';

		std::cout << "Server message:" << recv_line << std::endl; 
	}
	std::cout << "Exit" << std::endl;
}

char* get_message(char* send_line, int len, FILE *fp)
{
	std::cout << "Please string to server:";
	return (fgets(send_line, len, fp));
}
