/*************************************************************************
    > File Name: chat_server.cpp
    > Author: Jie Mo
    > Email: 582865471@vip.qq.com 
    > Github: JieTrancender 
    > Created Time: Wed Apr 27 07:30:12 2016
 ************************************************************************/

#include "chat_message.hpp"
#include <iostream>
#include <deque>
#include <cstdlib>
#include <list>
#include <vector>
#include <memory>
#include <set>
#include <utility>
#include <unistd.h>
#include <netinet/in.h>  //INADDR_ANY's header
#include <arpa/inet.h>  //inet_ntoa's header
#include <sys/types.h>
#include <sys/socket.h>
#include <thread>
#include <cstring>

namespace MCD
{
	using chat_message_queue = std::deque<ChatMessage>;
	class ChatParticipant
	{
	public:
		virtual ~ChatParticipant(){}

		virtual void deliver(const ChatMessage& msg) = 0;
	};

	using chat_participant_ptr = std::shared_ptr<ChatParticipant>;

	typedef struct arg
	{
		int connectfd;
		struct sockaddr_in lcient;
	}ARG;

	class ChatRoom
	{
		enum { max_recent_msgs = 100};

	public:
		void join(chat_participant_ptr participant)
		{
			m_participants.insert(participant);
			for (auto msg: m_recent_msgs)
			{
				participant->deliver(msg);
			}
		}

		void leave(chat_participant_ptr participant)
		{
			m_participants.erase(participant);
		}

		void deliver(const ChatMessage& msg)
		{
			m_recent_msgs.push_back(msg);
			while (m_recent_msgs.size() > max_recent_msgs)
			{
				m_recent_msgs.pop_front();
			}

			for (auto participant: m_participants)
			{
				participant->deliver(msg);
			}
		}

	private:
		std::set<chat_participant_ptr> m_participants;
		chat_message_queue m_recent_msgs;
	};

	class ChatSession : public ChatParticipant, public std::enable_shared_from_this<ChatSession>
	{
		enum { max_data_size = 4 + 1024 + 1};

	public:
		ChatSession(ARG* arg, ChatRoom& room):m_arg(arg), m_room(room){}

		void start()
		{
			m_room.join(shared_from_this());
			do_read_header();
		}

		void deliver(const ChatMessage& msg)
		{
			bool write_in_progress = !m_write_msgs.empty();
			m_write_msgs.push_back(msg);
			if (!write_in_progress)
			{
				do_write();
			}
		}

	private:
		void do_read_header()
		{
			auto self(shared_from_this());
			int num;

			if ((num = recv(m_arg->connectfd, m_read_msg.data(), ChatMessage::header_length, 0)) == 0)
			{
				std::cerr << "Wrong,client disconnectfd.\n";
				close(m_arg->connectfd);
				return;
			}

			if (m_read_msg.decode_header())
			{
				do_read_body();
			}
			else
			{
				m_room.leave(shared_from_this());
			}
			/*client_name[num] = '\0';
			std::cout << "You got aconnection from " << inet_ntoa(m_arg->client.sin_addr) << ",client's name is " << client_name << std::endl;*/
		}

		void do_read_body()
		{
			auto self(shared_from_this());
			int num;
			if ((num = recv(m_arg->connectfd, m_read_msg.body(), m_read_msg.body_length(), 0)) == 1)
			{
				m_room.leave(shared_from_this());
				std::cerr << "Wrong,failed to receive body data\n";
				return;
			}

			m_room.deliver(m_read_msg);
			do_read_header();
		}

		void do_write()
		{
			auto self(shared_from_this());
			
			send(m_arg->connectfd, m_write_msgs.front().data(), m_write_msgs.front().length(), 0);
			m_write_msgs.pop_front();
			if (!m_write_msgs.empty())
			{
				do_write();
			}
		}

		ARG *m_arg;
		ChatRoom& m_room;
		ChatMessage m_read_msg;
		chat_message_queue m_write_msgs;
	};

	class ChatServer
	{
		enum { max_log_num = 5, max_data_size = 1024 };

	public:
		ChatServer(const int port):m_port(port)
		{
			//创建套接字
			if ((m_listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
			{
				std::cerr << "Failed to create socket.\n";
				exit(1);
			}

			//设置套接字选项SO_REUSEADDR
			int opt = SO_REUSEADDR;
			setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

			//绑定套接字到相应地址，INADDR_ANY宏表示可接受来自本机的任何IP地址的客户连接
			m_server.sin_family = AF_INET;
			m_server.sin_port = htons(m_port);
			m_server.sin_addr.s_addr = htonl(INADDR_ANY);
			memset((void*)m_server.sin_zero, 0, sizeof(m_server.sin_zero));
			if (bind(m_listenfd, (struct sockaddr*)&m_server, sizeof(struct sockaddr)) == -1)
			{
				std::cerr << "Failed to bind address.\n";
				exit(1);
			}

			//监听网络连接
			if (listen(m_listenfd, max_log_num) == -1)
			{
				std::cerr << "Failed to listen.\n";
				exit(1);
			}

			socklen_t sin_size = sizeof(struct sockaddr_in);

			//接收客户连接，一旦连接，产生新线程服务客户，然后关闭连接套接字并继续接收下一个客户连接
			while(true)
			{
				if ((m_connectfd = accept(m_listenfd, (struct sockaddr*)&m_client, &sin_size)) == -1)
				{
					std::cerr << "Failed to accept.\n";
					exit(1);
				}
				
				//分配空间给arg，然后把连接套接字和客户地址信息复制给m_arg
				m_arg = new ARG;
				m_arg->connectfd = m_connectfd;
				memcpy((void*)m_arg, &m_client, sizeof(m_client));
				
				m_threads.push_back(std::thread(&ChatServer::interaction, this, m_arg));
			}
		}

		void interaction(ARG *info)
		{
			std::make_shared<ChatSession>(info, m_room)->start();
		}

	private:
		const int m_port;
		std::vector<std::thread> m_threads;
		struct sockaddr_in m_server;
		struct sockaddr_in m_client;
		int m_listenfd, m_connectfd;
		ARG *m_arg;
		ChatRoom m_room;
	};
}

int main(int argc, char** argv)
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage: " << argv[0] << " <port> [<port> ...]\n";
			return 1;
		}

		MCD::ChatServer server(std::atoi(argv[1]));


		/*std::list<ChatServer> servers;
		for (int i = 0; i < argc; ++i)
		{

		}*/

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}
