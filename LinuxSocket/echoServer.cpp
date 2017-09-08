/*************************************************************************
	> File Name: echoServer.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 14:02:56 2017
 ************************************************************************/

#include <iostream>
#include <cstring>

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

const int g_maxConn = 200;
int main()
{
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        cerr << "socket error" << endl;
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        cerr << "bind error" << endl;
        exit(1);
    }

    if (listen(listenfd, g_maxConn) == -1)
    {
        cerr << "listen error" << endl;
        exit(1);
    }

    char buf[512];
    int readBytes;
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

        cout << "client infomation:" << inet_ntoa(clientAddr.sin_addr) << ", " << ntohs(clientAddr.sin_port) << endl;

        memset(buf, 0, sizeof(buf));
        while ((readBytes = read(clientFd, buf, sizeof(buf))) > 0)
        {
            cout << buf;
            if (write(clientFd, buf, readBytes) == -1)
            {
                cerr << "write socket error" << endl;
                exit(1);
            }

            memset(buf, 0, sizeof(buf));
        }

        if (readBytes == 0)
        {
            cerr << "client connet closed..." << endl;
            close(clientFd);
        }
        else if (readBytes == -1)
        {
            cerr << "read socket error" << endl;
            exit(1);
        }
    }

    close(listenfd);
}

