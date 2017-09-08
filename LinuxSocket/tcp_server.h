/*************************************************************************
	> File Name: tcp_server.h
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 20:11:49 2017
 ************************************************************************/

#ifndef _TCP_SERVER_H
#define _TCP_SERVER_H

#include "tcp_socket.h"
#include "socket_exception.h"

class TcpClient;

class TcpServer : public TcpSocket
{
public:
    TcpServer(unsigned short int port, const char *ip = nullptr, int backlog = 200) throw(SocketException);
    ~TcpServer();

    void accept(TcpClient &client) const throw(SocketException);
    TcpClient accept() const throw(SocketException);
};
#endif
