/*************************************************************************
	> File Name: tcp_socket.h
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 19:59:22 2017
 ************************************************************************/

#ifndef _TCP_SOCKET_H
#define _TCP_SOCKET_H

class TcpSocket
{
protected:
    TcpSocket();
    virtual ~TcpSocket();

    bool create();
    bool bind(unsigned short int port, const char *ip = nullptr) const;
    bool listen(int backlog = 200) const;
    bool accept(TcpSocket& clientSocket) const;
    bool connect(unsigned short int port, const char *ip) const;

    bool reuseaddr() const;
    bool isValid() const
    {
        return (m_sockfd != -1);
    }

public:
    bool close();
    int getfd() const
    {
        return m_sockfd;
    }

    //flag:true -> SetNonBlock, false -> SetBlock
    bool setNonBlocking(bool flag) const;

protected:
    int m_sockfd;
};
#endif
