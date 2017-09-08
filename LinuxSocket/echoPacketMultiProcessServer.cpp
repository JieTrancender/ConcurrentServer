/*************************************************************************
	> File Name: echoMultiProcessServer.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 14:31:54 2017
 ************************************************************************/

#include "helper.h"

using namespace std;

const int g_maxConn = 200;

void echo(int clientfd, struct sockaddr_in clientAddr);
int main()
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        cerr << "socket error " << endl;
        exit(1);
    }
    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
    {
        cerr << "setsockopt error" << endl;
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        cerr << "bind error" << endl;
        exit(1);
    }

    if (listen(listenfd, g_maxConn) == -1)
    {
        cerr << "listen error" << endl;
        exit(1);
    }

    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    while (true)
    {
        int clientFd = accept(listenfd, (struct sockaddr*)&clientAddr, &addrLen);
        if (clientFd == -1)
        {
            cerr << "accept error" << endl;
            exit(1);
        }

        cout << "client information: " << inet_ntoa(clientAddr.sin_addr) << ", " << ntohs(clientAddr.sin_port) << endl;
        
        pid_t pid = fork();
        if (pid == -1)
        {
            cerr << "fork error" << endl;
            exit(1);
        }
        else if (pid > 0)
        {
            close(clientFd);
        }
        else if (pid == 0)
        {
            close(listenfd);
            echo(clientFd, clientAddr);
            exit(EXIT_SUCCESS);
        }
    }
    close(listenfd);
}

void echo(int clientfd, struct sockaddr_in clientAddr)
{
    struct Packet buf;
    int readBytes;
    while ((readBytes = readn(clientfd, &buf.msgLen, sizeof(buf.msgLen)))> 0)
    {
        //网络字节序 -> 主机字节序
        int lenHost = ntohl(buf.msgLen);
        readBytes = readn(clientfd, buf.data, lenHost);
        if (readBytes == -1)
        {
            cerr << "readn socket error" << endl;
            exit(1);
        }
        else if (readBytes != lenHost)
        {
            cerr << "client connect closed..." << endl;
            return;
        }
        cout << buf.data;

        if (writen(clientfd, &buf, sizeof(buf.msgLen) + lenHost) == -1)
        {
            cerr << "write socket error" << endl;
            exit(1);
        }
        memset(&buf, 0, sizeof(buf));
    }

    if (readBytes == -1)
    {
        cerr << "read socket error" << endl;
        exit(1);
    }
    else if (readBytes != sizeof(buf.msgLen))
    {
        cerr << "client connect closed..." << endl;
    }
}

