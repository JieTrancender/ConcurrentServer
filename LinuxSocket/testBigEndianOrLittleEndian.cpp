/*************************************************************************
	> File Name: testBigEndianOrLittleEndian.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 13:34:33 2017
 ************************************************************************/

#include<iostream>

using namespace std;

int main()
{
    int data = 0x12345678;

    char *p = (char*)&data;
    printf("%x, %x, %x, %x\n", p[0], p[1], p[2], p[3]);

    if (p[0] == 0x78)
    {
        cout << "当前系统为小端模式" << endl;
    }
    else if (p[0] == 0x12)
    {
        cout << "当前系统为小端模式大端模式" << endl;
    }
}

