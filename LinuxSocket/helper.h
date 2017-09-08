/*************************************************************************
	> File Name: helper.h
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 15:30:51 2017
 ************************************************************************/

#pragma once

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

struct Packet
{
    unsigned int msgLen;
    char data[1024];
};

ssize_t readn(int fd, void *buf, size_t count)
{
    size_t nLeft = count;
    ssize_t nRead = 0;
    char *pBuf = (char*)buf;

    while (nLeft > 0)
    {
        if ((nRead = read(fd, pBuf, nLeft)) < 0)
        {
            //如果读取操作被信号打断了，则说明还可以继续读
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        else if (nRead == 0)
        {
            return count - nLeft;
        }

        nLeft -= nRead;
        pBuf += nRead;
    }

    return count;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t nLeft = count;
    ssize_t nWritten = 0;
    char *pBuf = (char*)buf;

    while (nLeft > 0)
    {
        if ((nWritten = write(fd, pBuf, nLeft)) < 0)
        {
            //如果写入操作被信号打断了，则说明还可以继续写入
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        else if (nWritten == 0)
        {
            continue;
        }

        nLeft -= nWritten;
        pBuf += nWritten;
    }

    return count;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while (true)
    {
        int ret = recv(sockfd, buf, len, MSG_PEEK);
        //如果recv是由于被信号打断，则需要继续查看
        if (ret == -1 && errno == EINTR)
        {
            continue;
        }

        return ret;
    }
}

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
    int ret;
    int nRead = 0;
    int returnCount = 0;
    char *pBuf = (char*)buf;
    int nLeft = maxline;
    
    while (true)
    {
        ret = recv_peek(sockfd, pBuf, nLeft);
        if (ret <= 0)
        {
            return ret;
        }

        nRead = ret;
        for (int i = 0; i < nRead; ++i)
        {
            if (pBuf[i] == '\n')
            {
                ret = readn(sockfd, pBuf, i + 1);
                if (ret != i + 1)
                {
                    exit(EXIT_FAILURE);
                }

                return ret + returnCount;
            }
        }

        ret = readn(sockfd, pBuf, nRead);
        if (ret != nRead)
        {
            exit(EXIT_FAILURE);
        }

        pBuf += nRead;
        nLeft -= nRead;

        returnCount += nRead;
    }

    return -1;
}

int read_timeout(int fd, long waitSec)
{
    int returnValue = 0;
    if (waitSec > 0)
    {
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(fd, &readSet);

        struct timeval waitTime;
        waitTime.tv_sec = waitSec;
        waitTime.tv_usec = 0;
        do
        {
            returnValue = select(fd + 1, &readSet, nullptr, nullptr, &waitTime);
        } while (returnValue < 0 && errno == EINTR);

        if (returnValue == 0)
        {
            returnValue = -1;
            errno = ETIMEDOUT;
        }
        else if (returnValue == 1)
        {
            returnValue = 0;
        }
        //如果returnValue == -1并且errno != EINTR,则直接返回-1
    }

    return returnValue;
}

int write_timeout(int fd, long waitSec)
{
    int ret = 0;
    if (waitSec > 0)
    {
        fd_set writeSet;
        FD_ZERO(&writeSet);
        FD_SET(fd, &writeSet);

        struct timeval waitTime;
        waitTime.tv_sec = waitSec;
        waitTime.tv_usec = 0;
        do
        {
            ret = select(fd + 1, nullptr, &writeSet, nullptr, &waitTime);
        } while (ret < 0 && errno == EINTR);

        if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            ret = 0;
        }
    }

    return ret;
}

int accept_timeout(int fd, struct sockaddr_in *addr, long waitSec)
{
    int ret = 0;
    if (waitSec > 0)
    {
        fd_set acceptSet;
        FD_ZERO(&acceptSet);
        FD_SET(fd, &acceptSet);

        struct timeval waitTime;
        waitTime.tv_sec = waitSec;
        waitTime.tv_usec = 0;
        do
        {
            ret = select(fd + 1, &acceptSet, nullptr, nullptr, &waitTime);
        } while (ret < 0 && errno == EINTR);

        if (ret == 0)
        {
            errno = ETIMEDOUT;
            return -1;
        }
        else if (ret == -1)
        {
            return -1;
        }
    }

    socklen_t socklen = sizeof(struct sockaddr_in);
    if (addr != nullptr)
    {
        ret = accept(fd, (struct sockaddr*)addr, &socklen);
    }
    else
    {
        ret = accept(fd, nullptr, nullptr);
    }

    return ret;
}

//设置文件描述符fd为非阻塞/阻塞模式
bool setUnBlock(int fd, bool unBlock)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1)
    {
        return false;
    }

    if (unBlock)
    {
        flags |= O_NONBLOCK;
    }
    else
    {
        flags &= ~O_NONBLOCK;
    }

    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        return false;
    }

    return true;
}

int connect_timeout(int fd, struct sockaddr_in *addr, long waitSec)
{
    if (waitSec > 0)
    {
        setUnBlock(fd, true);
    }

    socklen_t addrlen = sizeof(struct sockaddr_in);
    //首次尝试进行连接
    int ret = connect(fd, (struct sockaddr*)addr, addrlen);
    //如果首次尝试失败并且errno == EINPROGRESS表示连接正在处理当中，则需要我们的介入
    if (ret < 0 && errno == EINPROGRESS)
    {
       fd_set connectSet;
        FD_ZERO(&connectSet);
        FD_SET(fd, &connectSet);

        struct timeval waitTime;
        waitTime.tv_sec = waitSec;
        waitTime.tv_usec = 0;
        do
        {
            ret = select(fd + 1, nullptr, &connectSet, nullptr, &waitTime);
        } while (ret < 0 && errno == EINTR);

        if (ret == -1)
        {
            return -1;
        }
        else if (ret == 0)
        {
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if (ret == 1)
        {
            //connectSet只有一个文件描述符，因此FD_ISSET的测试也就省了

            //套接字可写有两种情况：
            // 1. 连接建立成功
            // 2. 套接字产生错误，但此时select是正确的，因此错误信息没有保存在errno中，需要调用getsockopt获取

            int err;
            socklen_t errlen = sizeof(err);
            int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errlen);
            if (sockoptret == -1)
            {
                return -1;
            }

            if (err == 0)
            {
                ret = 0;
            }
            else
            {
                errno = err;
                ret = -1;
            }
        }
    }
    
    if (waitSec > 0)
    {
        setUnBlock(fd, false);
    }

    return ret;
}
