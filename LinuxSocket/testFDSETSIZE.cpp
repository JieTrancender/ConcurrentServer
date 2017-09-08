/*************************************************************************
	> File Name: testFDSETSIZE.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Fri Sep  8 15:51:05 2017
 ************************************************************************/

#include "tcp_server.h"
#include "helper.h"
#include <signal.h>

#include <iostream>

using namespace std;

int main()
{
    //signal(SIGPIPE, sigHandlerForSigPipe);
    signal(SIGPIPE, nullptr);
    try
    {
        TcpServer server(8000);
        int listenfd = server.getfd();
        
        struct sockaddr_in clientAddr;
        socklen_t addrlen;
        int maxfd = listenfd;
        fd_set rset;
        fd_set allset;
        FD_ZERO(&rset);
        FD_ZERO(&allset);
        FD_SET(listenfd, &allset);

        int client[FD_SETSIZE];
        for (int i = 0; i < FD_SETSIZE; ++ i)
        {
            client[i] = -1;
        }

        int maxi = 0;

        int count = 0;
        while (true)
        {
            rset = allset;
            int nReady = select(maxfd + 1, &rset, nullptr, nullptr, nullptr);
            if (nReady == -1)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                cerr << "select error" << endl;
                exit(1);
            }

            if (FD_ISSET(listenfd, &rset))
            {
                addrlen = sizeof(clientAddr);
                int connfd = accept(listenfd, (struct sockaddr *)&clientAddr, &addrlen);
                if (connfd == -1)
                {
                    cerr << "accept error" << endl;
                    exit(1);
                }

                int i;
                for (i = 0; i < FD_SETSIZE; ++i)
                {
                    if (client[i] < 0)
                    {
                        client[i] = connfd;
                        if (i > maxi)
                        {
                            maxi = i;
                        }
                        
                        break;
                    }
                }

                if (i == FD_SETSIZE)
                {
                    cerr << "too many clients" << endl;
                    exit(1);
                }

                cout << "client information: " << inet_ntoa(clientAddr.sin_addr) << ", " << ntohs(clientAddr.sin_port) << endl;
                cout << "count = " << ++count << endl;

                FD_SET(connfd, &allset);
                if (connfd > maxfd)
                {
                    maxfd = connfd;
                }

                if (--nReady <= 0)
                {
                    continue;
                }
            }

            //如果是已连接的套接口发生了可读事件
            for (int i = 0; i <= maxi; ++i)
            {
                if ((client[i] != -1) && FD_ISSET(client[i], &rset))
                {
                    char buf[512] = {0};
                    int readBytes = readline(client[i], buf, sizeof(buf));
                    if (readBytes == -1)
                    {
                        cerr << "readline error" << endl;
                        exit(1);
                    }
                    else if (readBytes == 0)
                    {
                        cerr << "client connect closed..." << endl;
                        FD_CLR(client[i], &allset);
                        close(client[i]);
                        client[i] = -1;
                    }
                    cout << buf;

                    if (writen(client[i], buf, readBytes) == -1)
                    {
                        cerr << "writen error" << endl;
                        exit(1);
                    }

                    if (--nReady <= 0)
                    {
                        break;
                    }
                }
            }
        }
    }
    catch (const SocketException &e)
    {
        cerr << e.what() << endl;
        cerr << "TcpServer error" << endl;
        exit(1);
    }
}

