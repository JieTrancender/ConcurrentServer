/*************************************************************************
	> File Name: tcp_client.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 20:35:18 2017
 ************************************************************************/

#include "tcp_client.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

TcpClient::TcpClient(unsigned short int port, const char *ip) throw(SocketException)
{
    if (false == create())
    {
        throw SocketException("tcp client create error");
    }

    if (false == connect(port, ip))
    {
        throw SocketException("tcp client connect error");
    }
}

TcpClient::TcpClient()
{}  // no content

TcpClient::TcpClient(int clientfd)
{
    m_sockfd = clientfd;
}

TcpClient::~TcpClient()
{}  // no content

static ssize_t readn(int fd, void *buf, size_t count);
static ssize_t writen(int fd, const void *buf, size_t count);

size_t TcpClient::send(const std::string& msg) const throw(SocketException)
{
    Packet buf;
    buf.msgLen = htonl(msg.size());
    strcpy(buf.data, msg.c_str());
    if (writen(m_sockfd, &buf, sizeof(buf.msgLen) + msg.size()) == -1)
    {
        throw SocketException("tcp client writen error");
    }

    return msg.size();
}

size_t TcpClient::receive(std::string &msg) const throw(SocketException)
{
    Packet buf = {0, 0};
    size_t readBytes = readn(m_sockfd, &buf.msgLen, sizeof(buf.msgLen));
    if (readBytes == static_cast<size_t>(-1))
    {
        throw SocketException("tcp client readn error");
    }
    else if (readBytes != sizeof(buf.msgLen))
    {
        throw SocketException("peer connect closed");
    }

    unsigned int lenHost = ntohl(buf.msgLen);
    readBytes = readn(m_sockfd, buf.data, lenHost);
    if (readBytes == static_cast<size_t>(-1))
    {
        throw SocketException("tcp client readn error");
    }
    else if (readBytes != lenHost)
    {
        throw SocketException("peer connect closed");
    }

    msg = buf.data;

    return msg.length();
}

size_t TcpClient::read(void *buf, size_t count) throw(SocketException)
{
    ssize_t readBytes = ::read(m_sockfd, buf, count);
    if (readBytes == static_cast<size_t>(-1))
    {
        throw SocketException("tcp client read error");
    }

    return static_cast<size_t>(readBytes);
}

void TcpClient::write(const void *buf, size_t count) throw(SocketException)
{
    if (::write(m_sockfd, buf, count) == -1)
    {
        throw SocketException("tcp client write error");
    }
}

size_t TcpClient::write(const char *msg) throw(SocketException)
{
    if (::write(m_sockfd, msg, strlen(msg)) == -1)
    {
        throw SocketException("tcp client write error");
    }

    return strlen(msg);
}

static ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nLeft = count;
    size_t nRead = 0;
    char *pBuf = reinterpret_cast<char*>(buf);

    while (nLeft > 0)
    {
        if ((nRead = read(fd, pBuf, nLeft)) < 0)
        {
            //如果读取操作是被信号打断，则说明可以继续读
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        else if (nRead == 0)
        {
            return count - nLeft;
        }

        nLeft -= nRead;
        pBuf += count;
    }

    return count;
}

static ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t nLeft = count;
    size_t nWritten = 0;
    const char *pBuf = reinterpret_cast<const char*>(buf);

    while (nLeft > 0)
    {
        if ((nWritten = write(fd, pBuf, nLeft)) < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        else if (nWritten == 0)
        {
            continue;
        }

        nLeft -= nWritten;
        pBuf += nWritten;
    }

    return count;
}
