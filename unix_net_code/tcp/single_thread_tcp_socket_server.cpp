/*************************************************************************
    > File Name: single_thread_tcp_socket_server.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Wed Apr 13 08:46:12 2016
 ************************************************************************/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <memory.h>
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h> //inet_ntoa's header
#include <netinet/in.h>  //INADDR_ANY's header
#include <sys/time.h>
#include <cstdio>

const unsigned short int c_port = 1234;
const int c_max_connection = 5;
const int c_max_data_size = 1024;

typedef struct
{
	int fd;
	char *name;
	struct sockaddr_in addr;
	char *data;
}Client;

void process_client(Client *client, char *recvbuf, int len);
void save_data(char *recvbuf, int len, char *data);

int main(int argc, char** argv)
{
	int max_fd, socket_fd, max_connection, num_ready, num;
	fd_set read_set, all_set;
	int listenfd, connectfd;
	struct sockaddr_in server;
	Client client[FD_SETSIZE];
	char recvbuf[c_max_data_size];

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

	if (bind(listenfd, (struct sockaddr*)&server, sizeof(server)) == -1)
	{
		std::cout << "Failed to bind address." << std::endl;
		exit(1);
	}

	if (listen(listenfd, c_max_connection) == -1)
	{
		std::cerr << "Failed to listen." << std::endl;
		exit(1);
	}

	socklen_t sin_size = sizeof(struct sockaddr_in);

	//initialize for select
	//将所监听的描述符上限(max_fd)设置为监听套接字(listenfd), 因为当前listenfd是唯一被监听的套接字
	max_fd = listenfd;
	//将最大连接的客户数(max_connection)设置为-1， 表示没有连接的客户
	max_connection = -1;

	for (int i = 0; i < FD_SETSIZE; ++i)
	{
		client[i].fd = -1;
	}

	//清空描述符集(all_set),用于存放所有被监听的描述符
	FD_ZERO(&all_set);

	//将listenfd放入all_set
	FD_SET(listenfd, &all_set);

	//将all_set的数据复制给read_set.read_set是可读描述符集合。由于调用select()函数，系统内核会根据I/O状态修改read_set内容，
	//所以必须用all_set来保存所有被监视的描述符，并且在每次调用select()函数之前，赋值给read_set
	while(true)
	{
		struct sockaddr_in addr;
		read_set = all_set;
		//调用select()函数。由于只监视可读的描述符，可写描述符集合和异常描述符集合被置为null，返回值为可读描述符的数量
		num_ready = select(max_fd + 1, &read_set, nullptr, nullptr, nullptr);

		//判断监听套接字是否可读，即是否有新的客户连接
		if (FD_ISSET(listenfd, &read_set))
		{
			//accept connection
			if ((connectfd = accept(listenfd, (struct sockaddr*)&addr, &sin_size)) == -1)
			{
				perror("Failed to accept");
				continue;
			}

			//在数据client寻找一空项，然后初始化新客户信息
			for (int i = 0; i < FD_SETSIZE; ++i)
			{
				if (client[i].fd < 0)
				{
					client[i].fd = connectfd;
					client[i].name = new char[c_max_data_size];
					client[i].addr = addr;
					client[i].data = new char[c_max_data_size];
					client[i].name[0] = '\0';
					client[i].data[0] = '\0';

					std::cout << "You got a connection from " << inet_ntoa(client[i].addr.sin_addr);
					break;
				}

				if (i == FD_SETSIZE - 1)
				{
					std::cerr << "Wrong,too many clients" << std::endl;
					break;
				}

				//将新产生的连接套接字放入all_set
				FD_SET(connectfd, &all_set);

				//如果新产生的连接套接字(connectfd)大于max_fd，则将maxfd置为connectfd
				if (connectfd > max_fd)
				{
					max_fd = connectfd;
				}

				//最大连接的客户数更新
				if (i > max_connection)
				{
					max_connection = i;
				}

				//如果没有其他可读的描述符，则重新调用select()函数，等待新的可读描述符
				if (--num_ready <= 0)
					continue;
			}

			//检查所有已连接的客户
			for (int i = 0; i <= max_connection; ++i)
			{
				//判断相应的连接套接字是否可读
				if ((socket_fd = client[i].fd) < 0)
				{
					continue;
				}

				//读客户发来的数据。如果字节数为0，表示客户关闭了连接
				if (FD_ISSET(socket_fd, &read_set))
				{
					if ((num = recv(socket_fd, recvbuf, c_max_data_size, 0)) == 0)
					{
						close(socket_fd);
						std::cout << "Client(" << client[i].name << ") closed connection,user's data:" << client[i].data << std::endl;
						FD_CLR(socket_fd, &all_set);
						client[i].fd = -1;
						delete client[i].name;
						delete client[i].data;
					}
					else
					{
						process_client(&client[i], recvbuf, num);
						if (--num_ready <= 0)
						{
							break;
						}
					}
				}
			}
		}
	}
	close(listenfd);
	return 0;
}

void process_client(Client *client, char *recvbuf, int len)
{
	char sendbuf[c_max_data_size];
	recvbuf[len - 1] = '\0';

	//判断是否是新连接
	if (strlen(client->name) == 0)
	{
		memcpy(client->name, recvbuf, len);
		std::cout << ", client's name is " << client->name << std::endl;
		return;
	}

	std::cout << "Received client(" << client->name << ") message:" << recvbuf << std::endl;
	save_data(recvbuf, len, client->data);

	for (int i = 0; i < len - 1; ++i)
	{
		sendbuf[i] = recvbuf[len - i - 2];
	}
	sendbuf[len - 1] = '\0';
	send(client->fd, sendbuf, strlen(sendbuf), 0);
}

void save_data(char *recvbuf, int len, char *data)
{
	int start = strlen(data);
	for (int i = 0; i < len; ++i)
	{
		data[start + i] = recvbuf[i];
	}
}
