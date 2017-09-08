/*************************************************************************
	> File Name: echoClient.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 14:18:44 2017
 ************************************************************************/

#include "helper.h"
#include <sys/select.h>

using namespace std;

void echoClient(int sockfd);

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        cerr << "socket error" << endl;
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8000);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        cerr << "connet error" << endl;
        exit(1);
    }

    echoClient(sockfd);
}

void echoClient(int sockfd)
{
    struct Packet buf;
    fd_set rset;
    //确保标准输入不会被重定向
    int fd_stdin = fileno(stdin);
    int maxfd = fd_stdin > sockfd ? fd_stdin : sockfd;

    while (true)
    {
        FD_ZERO(&rset);
        FD_SET(fd_stdin, &rset);
        FD_SET(sockfd, &rset);

        int nReady = select (maxfd + 1, &rset, nullptr ,nullptr, nullptr);
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
            memset(&buf, 0, sizeof(buf));
            if (fgets(buf.data, sizeof(buf.data), stdin) != nullptr)
            {
                unsigned int lenHost = strlen(buf.data);
                buf.msgLen = htonl(lenHost);

                if (writen(sockfd, &buf, sizeof(buf.msgLen) + lenHost) == -1)
                {
                    cerr << "writen socket error" << endl;
                    exit(1);
                }
            }
            else
            {
                break;
            }
        }

        if (FD_ISSET(sockfd, &rset))
        {
            memset(&buf, 0, sizeof(buf));
            int readBytes = readn(sockfd, &buf.msgLen, sizeof(buf.msgLen));
            if (readBytes == -1)
            {
                cerr << "readn socket error" << endl;
                exit(1);
            }
            else if (readBytes != sizeof(buf.msgLen))
            {
                cerr << "server connect closed..." << endl;
                exit(1);
            }
            else if (readBytes == sizeof(buf.msgLen))
            {
                int lenHost = ntohl(buf.msgLen);
                readBytes = readn(sockfd, buf.data, lenHost);
                if (readBytes == -1)
                {
                    cerr << "readn socket error" << endl;
                    exit(1);
                }
                else if (readBytes != lenHost)
                {
                    cerr << "server connect closed..." << endl;
                    exit(1);
                }

                cout << buf.data;
            }
        }
    }
}

