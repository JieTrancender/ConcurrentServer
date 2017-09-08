/*************************************************************************
	> File Name: testHostConversion.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 13:41:40 2017
 ************************************************************************/

#include<iostream>

#include <arpa/inet.h>

using namespace std;

int main()
{
    int localeData = 0x12345678;
    char *p = (char*)&localeData;
    printf("Begin: %0x %0x %0x %0x\n", p[0], p[1], p[2], p[3]);

    int inetData = htonl(localeData);
    p = (char*)&inetData;
    printf("After: %0x %0x %0x %0x\n", p[0], p[1], p[2], p[3]);

    if (p[0] == 0x12)
    {
        cout << "网络系统为大端模式" << endl;
    }
    else if (p[0] == 0x78)
    {
        cout << "网络系统为小端模式" << endl;
    }

    printf("host: %x, inet: %x\n", localeData, inetData);
}

