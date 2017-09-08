/*************************************************************************
	> File Name: tcp_socket.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 20:14:16 2017
 ************************************************************************/

#include "tcp_socket.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

TcpSocket::TcpSocket()
    : m_sockfd(-1)
    {}  // no content

TcpSocket::~TcpSocket()
{
    if (isValid())
    {
        ::close(m_sockfd);
    }
}

bool TcpSocket::create()
{
    if (isValid())
    {
        return false;
    }

    if ((m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        return false;
    }

    return true;
}

bool TcpSocket::bind(unsigned short int port, const char *ip) const
{
    if (!isValid())
    {
        return false;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (ip == nullptr)
    {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        addr.sin_addr.s_addr = inet_addr(ip);
    }

    if (::bind(m_sockfd, (const struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        return false;
    }

    return true;
}

bool TcpSocket::listen(int backlog) const
{
    if (!isValid())
    {
        return false;
    }

    if (::listen(m_sockfd, backlog) == -1)
    {
        return false;
    }

    return true;
}

bool TcpSocket::accept(TcpSocket &clientSocket) const
{
    if (!isValid())
    {
        return false;
    }

    clientSocket.m_sockfd = ::accept(this->m_sockfd, nullptr, nullptr);
    if (clientSocket.m_sockfd == -1)
    {
        return false;
    }

    return true;
}

bool TcpSocket::connect(unsigned short int port, const char *ip) const
{
    if (!isValid())
    {
        return false;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (::connect(m_sockfd, (const struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        return false;
    }

    return true;
}

bool TcpSocket::setNonBlocking(bool flag) const
{
    if (!isValid())
    {
        return false;
    }

    int opt = fcntl(m_sockfd, F_GETFL, 0);
    if (opt == -1)
    {
        return false;
    }

    if (flag)
    {
        opt |= O_NONBLOCK;
    }
    else
    {
        opt &= ~O_NONBLOCK;
    }

    if (fcntl(m_sockfd, F_SETFL, opt) == -1)
    {
        return false;
    }

    return true;
}

bool TcpSocket::reuseaddr() const
{
    if (!isValid())
    {
        return false;
    }

    int on = 1;
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        return false;
    }

    return true;
}

bool TcpSocket::close()
{
    if (!isValid())
    {
        return false;
    }

    ::close(m_sockfd);
    m_sockfd = -1;

    return true;
}
