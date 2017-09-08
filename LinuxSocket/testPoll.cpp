/*************************************************************************
	> File Name: testFDSETSIZE.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Fri Sep  8 15:51:05 2017
 ************************************************************************/

#include "tcp_server.h"
#include "helper.h"
#include <signal.h>
#include <poll.h>

#include <iostream>

using namespace std;

const int SETSIZE = 2048;

int main()
{
    //signal(SIGPIPE, sigHandlerForSigPipe);
    signal(SIGPIPE, nullptr);
    try
    {
        TcpServer server(8000);
        int listenfd = server.getfd();
        struct pollfd client[SETSIZE];

        for (int i = 0; i < SETSIZE; ++i)
        {
            client[i].fd = -1;
        }

        int maxi = 0;
        int count = 0;
        client[0].fd = server.getfd();
        client[0].events = POLLIN;
        while (true)
        {
            int nReady = poll(client, maxi + 1, -1);
            if (nReady == -1)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                cerr << "poll error" << endl;
                exit(1);
            }

            if (client[0].revents & POLLIN)
            {
                int connfd = accept(server.getfd(), nullptr, nullptr);
                if (connfd == -1)
                {
                    cerr << "accept error" << endl;
                    exit(1);
                }

                bool flags = false;
                for (int i = 1; i < SETSIZE; ++i)
                {
                    if (client[i].fd == -1)
                    {
                        client[i].fd = connfd;
                        client[i].events = POLLIN;
                        flags = true;
                        if (i > maxi)
                        {
                            maxi = i;
                        }

                        break;
                    }
                }

                if (!flags)
                {
                    cerr << "too many clients" << endl;
                    exit(1);
                }

                cout << "count = " << ++ count << endl;
                if (--nReady <= 0)
                {
                    continue;
                }
            }
            
            for (int i = 1; i <= maxi; ++i)
            {
                if (client[i].revents & POLLIN)
                {
                    char buf[512] = {0};
                    int readBytes = readline(client[i].fd, buf, sizeof(buf));
                    if (readBytes == 0)
                    {
                        cerr << "readline error" << endl;
                        exit(1);
                    }
                    else if (readBytes == 0)
                    {
                        cerr << "client connect closed..." << endl;
                        close(client[i].fd);
                        client[i].fd = -1;
                    }

                    cout << buf;
                    if (writen(client[i].fd, buf, readBytes) == -1)
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

