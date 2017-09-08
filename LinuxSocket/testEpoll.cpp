/*************************************************************************
	> File Name: testEpoll.cpp
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Fri Sep  8 18:56:35 2017
 ************************************************************************/

#include "helper.h"
#include "tcp_server.h"

#include <signal.h>
#include <sys/epoll.h>
#include <sys/wait.h>

#include <iostream>
#include <vector>

using namespace std;

void addFd(int epollfd, int fd, const uint32_t &events = EPOLLIN, bool et = false)
{
    struct epoll_event event;
    event.events = events;
    if (et)
    {
        //设置为边缘触发
        event.events |= EPOLLET;
    }

    event.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        cerr << "epoll_ctl_add error" << endl;
        exit(1);
    }
}

void delFd(int epollfd, int fd)
{
    struct epoll_event event;
    event.data.fd = fd;
    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &event) == -1)
    {
        cerr << "epoll_ctl_del error" << endl;
        exit(1);
    }
}

void sigHandler(int signo)
{
    while (waitpid(-1, nullptr, WNOHANG) > 0);
}

int main()
{
    //signal(SIGPIPE, sigHandlerForSigPipe);
    signal(SIGPIPE, sigHandler);

    try
    {
        TcpServer server(8000);
        int listenfd = server.getfd();
        int epollfd = epoll_create1(EPOLL_CLOEXEC);
        if (epollfd == -1)
        {
            cerr << "epoll_createl error" << endl;
            exit(1);
        }

        addFd(epollfd, listenfd, EPOLLIN, true);

        std::vector<struct epoll_event> events(16);
        char buf[512];
        int count = 0;

        while (true)
        {
            int nReady = epoll_wait(epollfd, &*events.begin(), (int)events.size(), -1);
            if (nReady == -1)
            {
                if (errno == EINTR)
                {
                    continue;
                }

                cerr << "epoll_wait error" << endl;
                exit(1);
            }

            if ((size_t)nReady == events.size())
            {
                events.resize(events.size() * 2);
            }

            for (int i = 0; i < nReady; ++i)
            {
                if (events[i].data.fd == listenfd)
                {
                    int connectfd = accept(listenfd, nullptr, nullptr);
                    if (connectfd == -1)
                    {
                        cerr << "accept error" << endl;
                        exit(1);
                    }

                    cout << "accept success..." << endl;
                    cout << "count = " << ++count << endl;
                    setUnBlock(connectfd, true);
                    addFd(epollfd, connectfd, EPOLLIN, true);
                }
                else if (events[i].events & EPOLLIN)
                {
                    int connectfd = events[i].data.fd;
                    if (connectfd < 0)
                    {
                        continue;
                    }

                    memset(buf, 0, sizeof(buf));
                    int ret = readline(connectfd, buf, sizeof(buf) - 1);
                    if (ret == -1)
                    {
                        cerr << "readline error" << endl;
                        exit(1);
                    }
                    else if (ret == 0)
                    {
                        cerr << "client connect closed..." << endl;
                        delFd(epollfd, connectfd);
                        close(connectfd);
                        continue;
                    }
                    cout << buf;
                    writen(connectfd, buf, strlen(buf));
                }
            }
        }
    }
    catch (const SocketException &e)
    {
        cerr << e.what() << endl;
        cerr << "TcpServer error" << endl;
        exit(1);
    }
}
