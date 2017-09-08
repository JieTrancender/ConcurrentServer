/*************************************************************************
	> File Name: testGetHostIp.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 17:52:50 2017
 ************************************************************************/

#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>

using namespace std;

int  gethostip(char *ip)
{
    struct hostent *hp = gethostent();
    if (hp == nullptr)
    {
        return -1;
    }

    strcpy(ip, inet_ntoa(*(struct in_addr*)hp->h_addr));
    return 0;
}

int main()
{
    char host[128] = {0};
    if (gethostname(host, sizeof(host)) == -1)
    {
        cerr << "gethostname error" << endl;
        exit(1);
    }

    cout << "host-name:" << host << endl;
    struct hostent *hp = gethostbyname(host);
    if (hp == nullptr)
    {
        cerr << "gethostbyname error" << endl;
        exit(1);
    }

    cout << "ip list:" << endl;
    for (int i = 0; hp->h_addr_list[i] != nullptr; ++i)
    {
        cout << '\t' << inet_ntoa(*(struct in_addr*)hp->h_addr_list[i]) << endl;
    }

    char ip[33] = {0};
    gethostip(ip);
    cout << "local-ip:" << ip << endl;
}

