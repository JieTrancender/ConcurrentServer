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

//UNP stand for Unix Net Program
namespace UNP
{
	struct Arg
	{
		int connectfd;
		struct sockaddr_in client;
	};

	class Server
	{
		enum { Port = 1024, BackLog = 5, MaxDataSize = 1024 };

	public:
		Server():m_sin_size(sizeof(struct sockaddr)), m_opt(SO_REUSEADDR),m_connectfd(0)
		{
			if (create_socket() == false)
			{
				throw "Failed to create socket.";
			}

			setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &m_opt, sizeof(m_opt));

			if (bind_address() == false)
			{
				throw "Failed to bind address.";
			}
			

			if (listen_connection() == false)
			{
				throw "Failed to listen.";
			}
			
			deal_connection();
		}

		~Server()
		{
			close(m_listenfd);
		}
		
		void deal_connection()
		{
			while (true)
			{
				if (accept_connection() == false)
				{
					throw "Failed to accept.";
				}

				m_arg = new Arg;
				m_arg->connectfd = m_connectfd;
				memcpy((void*)&m_arg->client, &m_client, sizeof(m_client));

				if (create_thread() == false)
				{
					throw "Failed to create thread.";
				}
			}
		}

	private:
		bool create_socket()
		{
			if ((m_listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
				return false;
			return true;
		}

		bool bind_address()
		{
			m_server.sin_family = AF_INET;
			m_server.sin_port = htons(Port);
			m_server.sin_addr.s_addr = htonl(INADDR_ANY);
			memset(m_server.sin_zero, 0, sizeof(m_server.sin_zero));

			if (bind(m_listenfd, (struct sockaddr*)&m_server, sizeof(m_server)) == -1)
			{
				return false;
			}
			return true;
		}

		bool listen_connection()
		{
			if (listen(m_listenfd, BackLog) == -1)
			{
				return false;
			}
			return true;
		}

		bool accept_connection()
		{
			if (m_connectfd = accept(m_listenfd, (struct sockaddr*)&m_client, &m_sin_size) == -1)
			{
				return false;
			}
			return true;
		}

		bool create_thread()
		{
			if (pthread_create(&m_thread, nullptr, UNP::Server::start_routine, (void*)m_arg))
			{
				return false;
			}
			return true;
		}

		void* start_routine(void *arg)
		{
			Arg *info;
			info = (Arg*)arg;

			process_client(info->connectfd, info->client);
			delete m_arg;
			pthread_exit(nullptr);
		}

		void process_client(int connectfd, struct sockaddr_in client)
		{
			int num_byte;
			char recvbuf[MaxDataSize], sendbuf[MaxDataSize], client_name[MaxDataSize];

			std::cout << "You got a connection from " << inet_ntoa(client.sin_addr);
			num_byte = recv(connectfd, client_name,  MaxDataSize, 0);
			if (num_byte == 0)
			{
				close(connectfd);
				std::cout << "Client disconnected." << std::endl;
				return;
			}
			client_name[num_byte - 1] = '\0';
			std::cout << ", client's name is " << client_name << std::endl;

			while (num_byte = recv(connectfd, recvbuf, MaxDataSize, 0))
			{
				recvbuf[num_byte] = '\0';
				std::cout << "Received client(" << client_name << ") message:" << recvbuf << std::endl;

				for (int i = 0; i < num_byte - 1; ++i)
				{
					sendbuf[i] = recvbuf[num_byte - i - 2];
				}
				sendbuf[num_byte - 1] = '\0';
				send(connectfd, sendbuf, strlen(sendbuf), 0);
			}
			close(connectfd);
		}

		int m_listenfd, m_connectfd, m_opt;
		pthread_t m_thread;
		Arg *m_arg;
		struct sockaddr_in m_server, m_client;
		socklen_t m_sin_size;
	};
};

int main(int argc, char** argv)
{
	UNP::Server my_server;
	my_server.deal_connection();
	return 0;
}

