/**
*#include <sys/epoll.h>
*int epoll_create(int size);  //最大监听为size - 1,本身占用一个
*int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
*  1. epoll_create函数返回的值
*  2.
*  EPOLL_CTL_ADD 注册新的fd到epfd中
*  EPOLL_CTL_MOD 修改已经注册fd的监听事件
*  EPOLL_CTL_DEL 从epfd中删除一个fd
*  3. 需要监听的fd
*  4. 指明需要监听的事
*    struct epoll_event {
*      __uint32_t events;
*      epoll_data_t data;
*    }
*int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>

const char* IPADDRESS = "127.0.0.1";
const int PORT = "3030";
const int MAXSIZE = 1024;
const int LISTENQ = 5;
const int FDSIZE = 1000;
const int EPOLLEVENTS = 100;

//创建套接字并进行绑定
static int socket_bind(const char* ip, int port);
//IO多路复用poll
static void do_epoll(int listenfd);
//事件处理
static void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char* buf);
//处理接收到的链接
static void handle_accept(int epollfd, int listenfd);
//读处理
static void do_read(int epollfd, int fd, char* buf);
//写处理
static void do_write(int epollfd, int fd, char* buf);
//添加事件
static void add_event(int epollfd, int fd, int state);
//修改事件
static void modify_event(int epollfd, int fd, int state);
//删除事件
static void delete_event(int epollfd, int fd, int state);

int main(int argc, char* argv[])
{
    int listenfd;
    listenfd = socket_bind(IPADDRESS, PORT);
    listen(listenfd, LISTENQ);
    do_epoll(listenfd);
    return 0;
}

static int socket_bind(const char* ip, int port)
{
    int listenfd;
    struct sockaddr_in serverAddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        fprintf(stderr, "%s\n", "socket error!");
        exit(1);
    }
    //memset(&serverAddr, 0, sizeof(serverAddr));
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serverAddr.sin_addr);
    serverAddr.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        fprintf(stderr, "%s\n", "bind error!");
        exit(1);
    }
    return listenfd;
}

static void do_epoll(int listenfd)
{
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    int ret;
    char buf[MAXSIZE];
    memset(buf, 0, strlen(buf));
    epollfd = epoll_create(FDSIZE);
    add_event(epollfd, listenfd, EPOLLIN);
    for ( ; ;)
    {
        ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
        handle_events(epollfd, events, ret, listenfd, buf);
    }
    close(epollfd);
}

static void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char* buf)
{
    int i;
    int fd;
    for (int i = 0; i < num; ++i)
    {
        fd = events[i].data.fd;
        if ((fd == listenfd) && (events[i].events & EPOLLIN))
        {
            handle_accept(epollfd, listenfd)
        }
        else if (events[i].events & EPOLLIN)
        {
            do_read(epollfd, fd, buf);
        }
        else if (events[i].events & EPOLLOUT)
        {
            do_write(epollfd, fd, buf);
        }
    }
}

static void handle_accept(int epollfd, int listenfd)
{
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t cliaddrlen;
    clifd = accept(listenfd, (struct sockaddr*)&cliaddr, &cliaddrlen);
    if (clifd == -)
    {
        fprintf(stderr, "%s\n", "accept error!");
    }
    printf("accept a new client: %s:%d\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
    add_event(epollfd, clifd, EPOLLIN);
}

static void do_read(int epollfd, int fd, char* buf)
{
    int nread;
    nread = read(fd, buf, MAXSIZE);
    if (nread == -1)
    {
        fprintf(stderr, "%s\n", "read error!");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else if (nread == 0)
    {
        fprintf(stderr, "client close!\n");
        close(fd);
        delete_event(epollfd, fd, EPOLLIN);
    }
    else
    {
        printf("read message: %s", buf);
        modify_event(epollfd, fd, EPOLLOUT);
    }
}

static void do_write(int epollfd, int fd, char* buf)
{
    int nwrite;
    nwrite = write(fd, buf, strlen(buf));
    if (nwrite == -1)
    {
        fprintf(stderr, "write error!");
        close(fd);
        delete_event(epollfd, fd, EPOLLOUT);
    }
    else
    {
        modify_event(epollfd, fd, EPOLLIN);
        memset(buf, 0, MAXSIZE);
    }
}

static void add_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

static void delete_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, &ev);
}

static void modify_event(int epollfd, int fd, int state)
{
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
}

/*class EpollEchoServer
{
public:
    EpollEchoServer(int port)
    :port_(port), listenfd_(socket_bind()), epollfd_(epoll_create(FDSIZE))
    {
        memset(buf, 0, strlen(buf));
    }
    
    int socket_bind()
    {
        int listenfd;
        struct sockaddr_in serverAddr;
        listenfd = socket(AF_INET, SOCK_STREAM, 0);
        if (listenfd == -1)
        {
            fprintf(stderr, "%s\n", "socket error!");
            exit(1);
        }
        //memset(&serverAddr, 0, sizeof(serverAddr));
        bzero(&serverAddr, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        inet_pton(AF_INET, ip_, &serverAddr.sin_addr);
        serverAddr.sin_port = htons(port);
        if (bind(listenfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
        {
            fprintf(stderr, "%s\n", "bind error!");
            exit(1);
        }
        return listenfd;
    }

    void do_epoll()
    {
        
    }

static void do_epoll(int listenfd)
{
    int epollfd;
    //struct epoll_event events[EPOLLEVENTS];
    int ret;
    //char buf[MAXSIZE];
    //memset(buf, 0, strlen(buf));
    //epollfd = epoll_create(FDSIZE);
    add_event(epollfd, listenfd, EPOLLIN);
    for ( ; ;)
    {
        ret = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
        handle_events(epollfd, events, ret, listenfd, buf);
    }
    close(epollfd);
}

private:
    static const char* ip_ = "127.0.0.1";
    int port_;
    int listenfd_;
    struct epoll_event events[EPOLLEVENTS];
    int epollfd_;
    char buf_[MAXSIZE];
};
*/
