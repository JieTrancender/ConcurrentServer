/*************************************************************************
	> File Name: echoSocketServer.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 21:25:27 2017
 ************************************************************************/

#include "tcp_server.h"
#include "tcp_client.h"
#include "socket_exception.h"

#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>

using namespace std;

void sigHandler(int signo)
{
    while (waitpid(-1, nullptr, WNOHANG) > 0)
    {}  // no content
}

int main()
{
    signal(SIGCHLD, sigHandler);
    signal(SIGPIPE, SIG_IGN);

    try
    {
        TcpServer server(8000);
        std::string msg;
        
        while (true)
        {
            TcpClient client = server.accept();
            pid_t pid = fork();
            if (pid == -1)
            {
                cerr << "fork error" << endl;
            }
            else if (pid > 0)
            {
                client.close();
            }
            else if (pid == 0)
            {
                try
                {
                    while (true)
                    {
                        client.receive(msg);
                        cout << msg << endl;
                        client.send(msg);
                    }
                }
                catch (const SocketException &e)
                {
                    cerr << e.what() << endl;
                    exit(EXIT_FAILURE);
                }
                exit(EXIT_SUCCESS);
            }
        }
    }
    catch (const SocketException& e)
    {
        cerr << e.what() << endl;
        exit(EXIT_FAILURE);
    }
}
