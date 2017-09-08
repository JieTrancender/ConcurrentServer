/*************************************************************************
	> File Name: testBind.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 13:53:13 2017
 ************************************************************************/

#include<iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

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
    //inet_aton("127.0.0.1", &addr.sin_addr);
    //addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(listenfd, (const struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        cerr << "bind error" << endl;
        exit(1);
    }
    else
    {
        cout << "bind success" << endl;
    }
}

