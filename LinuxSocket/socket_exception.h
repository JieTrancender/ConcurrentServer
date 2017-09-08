/*************************************************************************
	> File Name: socket_exception.h
	> Author: JieTrancender
	> Mail: jie-email@jie-trancender.org
	> Created Time: Thu Sep  7 20:59:44 2017
 ************************************************************************/

#ifndef _SOCKET_EXCEPTION_H
#define _SOCKET_EXCEPTION_H

#include <string>
#include <cstring>
#include <errno.h>

class SocketException
{
public:
    SocketException(const std::string &message = std::string())
    : m_msg(message)
    {} // no content

    std::string what() const
    {
        if (errno == 0)
        {
            return m_msg;
        }

        return m_msg + ": " + strerror(errno);
    }

private:
    std::string m_msg;
};
#endif
