/*************************************************************************
	> File Name: echoSocketClient.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 21:42:19 2017
 ************************************************************************/

#include "tcp_client.h"
#include "socket_exception.h"

#include <signal.h>

#include <string>
#include <iostream>
#include <cstdio>

using namespace std;

int main()
{
    signal(SIGPIPE, SIG_IGN);
    try
    {
        TcpClient client(8000, "127.0.0.1");
        string msg;

        while (getline(cin, msg))
        {
            client.send(msg);
            msg.clear();
            client.receive(msg);
            cout << msg << endl;
            msg.clear();
        }
    }
    catch (const SocketException &e)
    {
        cerr << e.what() << endl;
    }
}
