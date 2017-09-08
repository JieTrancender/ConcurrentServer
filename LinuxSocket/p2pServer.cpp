/*************************************************************************
	> File Name: p2pServer.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 15:01:19 2017
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

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
    int clientfd = accept(listenfd, (struct sockaddr*)&clientAddr, &addrLen);
    if (clientfd == -1)
    {
        cerr << "accept error" << endl;
        exit(1);
    }
    close(listenfd);

    cout << "client information:" << inet_ntoa(clientAddr.sin_addr) << ", " << ntohs(clientAddr.sin_port) << endl;

    char buf[512] = {0};
    pid_t pid = fork();
    if (pid == -1)
    {
        cerr << "fork error " << endl;
        exit(1);
    }
    else if (pid > 0)
    {
        int readBytes;
        while ((readBytes = read(clientfd, buf, sizeof(buf))) > 0)
        {
            cout << buf;
            memset(buf, 0, sizeof(buf));
        }
        if (readBytes == 0)
        {
            cout << "client connect closed..." << endl;
        }
        else if (readBytes == -1)
        {
            cerr << "read socket error" << endl;
            exit(1);
        }
        //通知子进程退出
        kill(pid, SIGUSR1);
    }
    else if (pid == 0)
    {
        //signal(SIGUSR1, sigHandler);
        signal(SIGUSR1, nullptr);
        while (fgets(buf, sizeof(buf), stdin) != nullptr)
        {
            if (write(clientfd, buf, strlen(buf)) == -1)
            {
                cerr << "write socket error" << endl;
                exit(1);
            }
            memset(buf, 0, sizeof(buf));
        }
    }
    close(clientfd);
    exit(EXIT_SUCCESS);

}
