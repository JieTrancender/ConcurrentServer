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
#include <arpa/inet.h>
#include <netinet/in.h>

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
