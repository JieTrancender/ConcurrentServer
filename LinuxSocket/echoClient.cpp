/*************************************************************************
	> File Name: echoClient.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 14:18:44 2017
 ************************************************************************/

#include<iostream>
#include <cstring>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

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

    char buf[512];
    while (fgets(buf, sizeof(buf), stdin) != nullptr)
    {
        if (write(sockfd, buf, strlen(buf)) == -1)
        {
            cerr << "write error" << endl;
            exit(1);
        }
        memset(buf, 0, sizeof(buf));
        int readBytes = read(sockfd, buf, sizeof(buf));
        if (readBytes == 0)
        {
            cout << "server connet closed..." << endl;
            break;
        }
        else if (readBytes == -1)
        {
            cerr << "read socket error" << endl;
            exit(1);
        }
        cout << buf;
        memset(buf, 0, sizeof(buf));
    }
    close(sockfd);

}

