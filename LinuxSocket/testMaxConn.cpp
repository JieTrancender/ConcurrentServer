/*************************************************************************
	> File Name: testMaxConn.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Fri Sep  8 16:24:06 2017
 ************************************************************************/


#include "helper.h"
#include <iostream>

using namespace std;

int main()
{
    int count = 0;
    while (true)
    {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1)
        {
            sleep(5);
            cerr << "socket error" << endl;
            exit(1);
        }

        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(8000);
        serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        int ret = connect_timeout(sockfd, &serverAddr, 5);
        if (ret == -1 && errno == ETIMEDOUT)
        {
            cerr << "timeout..." << endl;
            cerr << "connect_timeout error" << endl;
            exit(1);
        }
        else if (ret == -1)
        {
            cerr << "connect_timeout error" <<endl;
            exit(1);
        }

        struct sockaddr_in peerAddr;
        socklen_t peerlen = sizeof(peerAddr);
        if (getpeername(sockfd, (struct sockaddr*)&peerAddr, &peerlen) == -1)
        {
            cerr << "getpeername error" << endl;
            exit(1);
        }

        cout << "server information: " << inet_ntoa(peerAddr.sin_addr) << ", " << ntohs(peerAddr.sin_port) << endl;
        cout << "count = " << ++count << endl;
    }
}
