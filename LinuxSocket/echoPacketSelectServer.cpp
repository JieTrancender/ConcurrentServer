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
    fd_set rset;
    //确保标准输入不会被重定向
    int fd_stdin = fileno(stdin);
    int maxfd = fd_stdin > clientfd ? fd_stdin : clientfd;
    while (true)
    {
        FD_ZERO(&rset);
        FD_SET(fd_stdin, &rset);
        FD_SET(clientfd, &rset);
        int nReady = select(maxfd + 1, &rset, nullptr, nullptr, nullptr);
        if (nReady == -1)
        {
            cerr << "select error" << endl;
            exit(1);
        }
        else if (nReady == 0)
        {
            continue;
        }

        if (FD_ISSET(fd_stdin, &rset))
        {
            memset(buf.data, 0, sizeof(buf));
            if (fgets(buf.data, sizeof(buf.data), stdin) == nullptr)
            {
                break;
            }
            unsigned int lenHost = strlen(buf.data);
            buf.msgLen = htonl(lenHost);

            if (writen(clientfd, &buf, sizeof(buf.msgLen) + lenHost) == -1)
            {
                cerr << "writen socket error" << endl;
                exit(1);
            }
        }

        if (FD_ISSET(clientfd, &rset))
        {
            memset(&buf, 0, sizeof(buf));
            int readBytes = readn(clientfd, &buf.msgLen, sizeof(buf.msgLen));
            if (readBytes == 0)
            {
                break;
            }
            else if (readBytes == -1)
            {
                cerr << "readn socket error" << endl;
                exit(1);
            }

            int lenHost = ntohl(buf.msgLen);
            readBytes = readn(clientfd, buf.data, lenHost);
            if (readBytes == -1)
            {
                cerr << "readn socket error" << endl;
                exit(1);
            }
            else if (readBytes != lenHost)
            {
                cerr << "client closed..." << endl;
                exit(1);
            }

            cout << buf.data;
        }
    }
}
