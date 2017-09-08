/*************************************************************************
	> File Name: testIpConversion.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 13:47:23 2017
 ************************************************************************/

#include<iostream>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main()
{
    cout << inet_addr("192.168.139.137") << endl;

    struct in_addr address;
    address.s_addr = inet_addr("192.168.139.137");
    cout << inet_ntoa(address) << endl;

    memset(&address, 0, sizeof(address));
    inet_aton("127.0.0.1", &address);
    cout << address.s_addr << endl;
    cout << inet_ntoa(address) << endl;
}
