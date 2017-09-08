/*************************************************************************
	> File Name: tcp_server.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 21:17:24 2017
 ************************************************************************/

#include "tcp_server.h"
#include "tcp_client.h"

TcpServer::TcpServer(unsigned short int port, const char *ip, int backlog) throw(SocketException)
{
    if (create() == false)
    {
        throw SocketException("tcp server create error");
    }

    if (reuseaddr() == false)
    {
        throw SocketException("tcp server reuseaddr error");
    }

    if (bind(port, ip) == false)
    {
        throw SocketException("tcp server bind error");
    }

    if (listen(backlog) == false)
    {
        throw SocketException("tcp server listen error");
    }
}

TcpServer::~TcpServer()
{}  // no content

void TcpServer::accept(TcpClient &client) const throw(SocketException)
{
    if (TcpSocket::accept(client) == -1)
    {
        throw SocketException("tcp server accept error");
    }
}

TcpClient TcpServer::accept() const throw(SocketException)
{
    TcpClient client;
    if (TcpSocket::accept(client) == -1)
    {
        throw SocketException("tcp server accept error");
    }

    return client;
}
