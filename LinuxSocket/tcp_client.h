/*************************************************************************
	> File Name: tcp_client.h
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 20:07:23 2017
 ************************************************************************/

#ifndef _TCP_CLIENT_H
#define _TCP_CLIENT_H

#include "tcp_socket.h"
#include "socket_exception.h"

class TcpClient : public TcpSocket
{
private:
    struct Packet
    {
        unsigned int msgLen;
        char data[1024];
    };

public:
    TcpClient(unsigned short int port, const char *ip) throw(SocketException);
    TcpClient();
    TcpClient(int clientfd);
    ~TcpClient();

    size_t send(const std::string& msg) const throw(SocketException);
    size_t receive(std::string& msg) const throw(SocketException);
    size_t read(void *buf, size_t count) throw(SocketException);
    void write(const void *buf, size_t count) throw(SocketException);
    size_t write(const char *msg) throw(SocketException);
};
#endif
