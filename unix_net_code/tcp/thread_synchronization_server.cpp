/*************************************************************************
    > File Name: thread_synchronization_server.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Thu Apr 14 14:59:45 2016
 ************************************************************************/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <pthread.h>
#include <arpa/inet.h>

const int c_max_thread = 3;
const unsigned short int c_port = 1234;
const int c_max_data_size = 1024;

pthread_mutex_t request_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t get_request = PTHREAD_COND_INITIALIZER;

int num_request = 0;

int socket_fd;
struct sockaddr_in server;
struct sockaddr_in client;
socklen_t sin_size;

struct request
{
	char info[c_max_data_size];
	struct request *next;
};

struct request *request_first = nullptr;
struct request *request_last = nullptr;

//将数据请求添加到请求队列
void add_request(char *info, pthread_mutex_t *p_mutex, pthread_cond_t *p_cond);

//从请求队列中获取一个请求
struct request* get_request(pthread_mutex_t *p_mutex);

//处理客户请求
void deal_request(struct request *a_request, int threaed_id);

//处理客户请求的线程所执行的函数
void* deal_request_loop(void *data);

int main(int argc, char** argv)
{
	int thread_id[c_max_thread];
	pthread_t p_thread[c_max_thread];
	int num;
	char msg[c_max_data_size];

	for (int i = 0; i < c_max_thread; ++i)
	{
		thread_id[i] = i;
		pthread_create(&pthread[i], nullptr, deal_request_loop, (void*)&thread_id[i]);
	}

	if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		std::cerr << "Failed to create socket." << std::endl;
		exit(1);
	}

	int opt = SO_REUSEADDR;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	server.sin_family = AF_INET;
	server.sin_port = htons(c_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(server.sin_zero, 0, sizeof(server.sin_zero));

	if (bind(socket_fd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1)
	{
		std::cerr << "Failed to bind." << std::endl;
		exit(1);
	}

	sin_size = sizeof(struct sockaddr);

	while(true)
	{
		num = recvfrom(socket_fd, msg, c_max_data_size, 0, (struct sockaddr*)&client, &sin_size);
		if (num < 0)
		{
			std::cerr << "Failed to use recvfrom." << std::endl;
			exit(1);
		}
		msg[num] = '\0';

		std::cout << "You got a message (" << msg << ") from " << inet_ntoa(client.sin_addr) << std::endl;

		add_request(msg, &list_mutex, &get_request);

		if (!strcmp(msg, "quit"))
		{
			for (int i = 0; i < c_max_thread; ++i)
			{
				pthread_join(p_thread[i], nullptr);
			}
			break;
		}
	}
	close(socket_fd);
	return 0;
}

void add_request(char *info, pthread_mutex_t *p_mutex, pthread_cond_t *p_cond)
{
	request *new_request = new request;
	if (new_request == nullptr)
	{
		std::cerr << "Failed to create new request." << std::endl;
		return;
	}
	memcpy(new_request->info, info, c_max_data_size);
	new_request->next = nullptr;

	pthread_mutex_lock(p_mutex);

	if (num_request == 0)
	{
		request_first = new_request;
		reqeest_last = new_request;
	}
	else
	{
		request_last->next = new_request;
		request_last = new_request;
	}
	
	++num_request;
	
	pthread_mutex_unlock(p_mutex);
	pthread_cond_singal(p_cond);
}

struct request* get_request(pthread_mutex_t *p_mutex)
{
	request *a_request;
	
	pthread_mutex_lock(p_mutex);

	if (num_request > 0)
	{
		a_request = request_first;
		request_first = a_request->next;

		if (request_first == nullptr)
		{
			request_last = nullptr;
		}

		--num_request;
	}
	else
	{
		a_request = nullptr;
	}

	pthread_mutex_unlock(p_mutex);
	return a_request;
}

void deal_request(struct request *a_request, int thread_id)
{
	char msg[c_max_data_size];

	if (a_request)
	{
		std::cout << "Thread '" << thread_id << "' deal request " << a_request->info << std::endl;
		fflush(stdout);
		sprintf(msg, "Thread '%d' deal your request '%s'\n", thread_id ,a_request->info);
		sendto(socket_fd, msg, strlen(msg), (struct sockaddr*)&client, sin_size);
	}
}

void* deal_request_loop(void *data)
{
	struct request *a_request;
	int thread_id = *((int*)data);

	pthread_mutex_lock(&request_mutex);

	while(true)
	{
		if (num_request > 0)
		{
			a_request = get_request(&list_mutex);
			if (a_request)
			{
				pthread_mutex_unlock(&list_mutex);
				delete a_request;
			}
		}
		else
		{
			pthread_cond_wait(&get_request, &request_mutex);
		}
	}
}
