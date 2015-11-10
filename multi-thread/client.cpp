/*************************************************************************
 > File Name: client.cpp
# Author: Jie Mo
# mail: 582865471@vip.qq.com
# Github: JieTrancender
# Created Time: 2015年11月08日 星期日 16时01分04秒
 ************************************************************************/
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

using namespace std;

const int c_port = 1234;
const int c_data_size = 100;

//声明process()函数，用于连接厚与服务器交互
void process(FILE *fp, int sockfd);

//声明getMessage()函数，用于提示并获得客户输入字符串
char* getMessage(char *sendline, int len, FILE *fp);

int main(int argc, char **argv)
{
	int fd;	//files描述符
	struct hostent *he;
	struct sockaddr_in server;

	//检查用户输入，如果输入不正确，提示用户正确输入
	if(argc != 2)
	{
		cout << "Usage: " << argv[0] <<"<IP Address>" << endl;
		exit(1);
	}

	//通过字符串形式的IP地址获得服务器的地址信息
	if((he = gethostbyname(argv[1])) == nullptr)
	{
		cout << "Gethostbyname failed !" << endl;
		exit(1);
	}

	//参数TCP套接字
	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		cout << "Socket failed !" << endl;
		exit(1);
	}

	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(c_port);
	server.sin_addr = *((struct in_addr *)he->h_addr);

	//连接服务器
	if(connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) ==-1)
	{
		cout << "Connect failed !" << endl;
		exit(1);
	}

	//调用process函数，将标准输入和套接字作为参数
	process(stdin, fd);

	//关闭套接字
	close(fd);
}

char* getMessage(char *sendline, int len, FILE *fp)
{
	//提示并获得客户输入字符串
	cout << "Input string to server: " << endl;
	return (fgets(sendline, c_data_size, fp));
}

void process(FILE *fp, int sockfd)
{
	char sendline[c_data_size], recvline[c_data_size];
	int numbytes;
	cout << "Connected to server ." << endl;
	cout << "Input name: ";
	if(fgets(sendline, c_data_size, fp) == nullptr)
	{
		cout << "Exit ." << endl;
		return;
	}

	//发送用户输入的字符串到服务器
	send(sockfd, sendline, strlen(sendline) - 1, 0);

	//重复从标准输入获得用户输入的字符串直到用户输入ctrl + D
	while(getMessage(sendline, c_data_size, fp) != nullptr)
	{
		send(sockfd, sendline, strlen(sendline) - 1, 0);	//-1去除回车
		//接收并显示来自服务器的信息
		if((numbytes = recv(sockfd, recvline, c_data_size, 0)) == 0)
		{
			cout << "Server terminated ." << endl;
			return;
		}
		recvline[numbytes] = '\0';
		cout << "Server Message: " << recvline << endl;
	}
	cout << "Exit ." << endl;
}
