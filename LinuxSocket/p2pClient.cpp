/*************************************************************************
	> File Name: p2pClient.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 15:30:17 2017
 ************************************************************************/

#include "helper.h"
#include <signal.h>

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
        cerr << "connect error" << endl;
        exit(1);
    }

    char buf[512] = {0};
    pid_t pid = fork();

    if (pid == -1)
    {
        cerr << "fork error" << endl;
        exit(1);
    }
    else if (pid > 0)
    {
        int readBytes;
        while (readBytes = read(sockfd, buf, sizeof(buf)) > 0)
        {
            cout << buf;
            memset(buf, 0, sizeof(buf));
        }
        if (readBytes == 0)
        {
            cout << "server connect closed..." << endl;
        }
        else if (readBytes == -1)
        {
            cerr << "read socket error" << endl;
            exit(1);
        }
        kill(pid, SIGUSR1);
    }
    else if (pid == 0)
    {
        signal(SIGUSR1, nullptr);
        while (fgets(buf, sizeof(buf), stdin) != nullptr)
        {
            if (write(sockfd, buf, strlen(buf)) == -1)
            {
                cerr << "write error" << endl;
                exit(1);
            }
            memset(buf, 0, sizeof(buf));
        }
    }
    close(sockfd);
    exit(EXIT_SUCCESS);
}
