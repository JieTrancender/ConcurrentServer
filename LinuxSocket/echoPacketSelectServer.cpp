/*************************************************************************
	> File Name: echoMultiProcessServer.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 14:31:54 2017
 ************************************************************************/

#include "helper.h"

using namespace std;

const int g_maxConn = 200;

int main()
{
    struct sockaddr_in clientAddr;
    socklen_t addrLen;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        cerr << "socket error" << endl;
        exit(1);
    }

    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) ==-1)
    {
        cerr << "setsocketopt error" << endl;
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listenfd, (const struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        cerr << "bind error" << endl;
        exit(1);
    }

    if (listen(listenfd, g_maxConn) == -1)
    {
        cerr << "listen error" << endl;
        exit(1);
    }

    int maxfd = listenfd;
    fd_set rset;
    fd_set allset;
    FD_ZERO(&rset);
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    //用来保存已连接的客户端套接字
    int client[FD_SETSIZE];
    for (int i = 0; i < FD_SETSIZE; ++i)
    {
        client[i] = -1;
    }

    int maxi = 0;

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
        else if (nReady == 0)
        {
            //nReady == 0表示超时，但是此处是一定不会发生的
            continue;
        }

        if (FD_ISSET(listenfd, &rset))
        {
            addrLen = sizeof(clientAddr);
            int connfd = accept(listenfd, (struct sockaddr*)&clientAddr, &addrLen);
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

            cout << "client information:" << inet_ntoa(clientAddr.sin_addr) << ", " << ntohs(clientAddr.sin_port) << endl;

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

        for (int i = 0; i <= maxi; ++i)
        {
            if ((client[i] != -1) && FD_ISSET(client[i], &rset))
            {
                Packet buf;
                memset(&buf, 0, sizeof(buf));

                int readBytes = readn(client[i], &buf.msgLen, sizeof(buf.msgLen));
                if (readBytes == -1)
                {
                    cerr << "readn error" << endl;
                    exit(1);
                }
                else if (readBytes == 0)
                {
                    cerr << "client connect closed..." << endl;
                    FD_CLR(client[i], &allset);
                    close(client[i]);
                    client[i] = -1;
                    continue;
                }

                int lenHost = htonl(buf.msgLen);
                readBytes = readn(client[i], buf.data, lenHost);
                if (readBytes == -1)
                {
                    cerr << "readn error" << endl;
                    exit(1);
                }
                else if (readBytes != lenHost)
                {
                    cerr << "client connect closed..." << endl;
                    FD_CLR(client[i], &allset);
                    close(client[i]);
                    client[i] = -1;
                    continue;
                }

                sleep(4);
                cout << buf.data;

                if (writen(client[i], &buf, sizeof(buf.msgLen) + lenHost) == -1)
                {
                    cerr << "writen error" << endl;
                    exit(1);
                }

                if (--nReady <= 0)
                break;
            }
        }
    }
}
