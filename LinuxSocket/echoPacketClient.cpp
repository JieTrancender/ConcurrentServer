/*************************************************************************
	> File Name: echoClient.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 14:18:44 2017
 ************************************************************************/

#include "helper.h"

using namespace std;

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

    struct Packet buf;
    memset(&buf, 0, sizeof(buf));
    while (fgets(buf.data, sizeof(buf.data), stdin) != nullptr)
    {
        unsigned int lenHost = strlen(buf.data);
        //将本机字节序转为网络字节序(大端)
        buf.msgLen = htonl(lenHost);
        if (writen(sockfd, &buf, sizeof(buf.msgLen) + lenHost) == -1)
        {
            cerr << "writen socket error" << endl;
            exit(1);
        }

        memset(&buf, 0, sizeof(buf));
        ssize_t readBytes = readn(sockfd, &buf.msgLen, sizeof(buf.msgLen));
        if (readBytes == -1)
        {
            cerr << "readn socket error" << endl;
            exit(1);
        }
        else if (readBytes != sizeof(buf.msgLen))
        {
            cerr << "server connect closed..." << endl;
            break;
        }

        lenHost = ntohl(buf.msgLen);
        readBytes = readn(sockfd, buf.data, lenHost);
        if (readBytes == -1)
        {
            cerr << "read socket error" << endl;
            exit(1);
        }
        else if (readBytes != lenHost)
        {
            cerr << "server connect closed..." << endl;
            break;
        }

        cout << buf.data;
        memset(&buf, 0, sizeof(buf));
    }

    close(sockfd);

}

