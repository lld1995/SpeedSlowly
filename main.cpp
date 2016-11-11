#include "stdlib.h"
#include "sys/types.h"
#include <sys/socket.h> 
#include "netinet/in.h"
#include "string.h"
#include "arpa/inet.h"
#include<unistd.h>
#include<pthread.h>
#include <iostream>
#include <cstdio>

#include <sys/stat.h>
#define MAXFILE 65535
using namespace std;
void send_and_recv(int sockfd, char* url, char* fun_type, char* accept_type, char* ip, int port, char* body, char* connection_type);
void* download(void* ptr);
//sockfd表示TCP链接的套接字，url请求服务的相对或者绝对地址，fun_type请求方法，accept_type接受类 型，ip,port请求的服务器的地址和端口，file_loc下载文件存放位置，body请求的主体，connection_type用来指定 connection的类型

int main()
{
	for (int i = 0; i < 150; i++)
	{
		int err;
		pthread_t ntid;
		err = pthread_create(&ntid, NULL, download, NULL);
		if (err) {
			cout << "create keepAlive failed";
		}
	}
	while (1) {
		sleep(1000);
	}
}
void* download(void* ptr)
{
	while (1)
	{
		int sockfd;
		struct sockaddr_in serv_socket;
		int port = 80;
		char ip[] = "211.84.144.7"; //ip地址，可以通过gethostbyname来获取
		bzero(&serv_socket, sizeof(struct sockaddr_in));
		serv_socket.sin_family = AF_INET;
		serv_socket.sin_port = htons(port);
		inet_pton(AF_INET, ip, &serv_socket.sin_addr);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		int flag = connect(sockfd, (struct sockaddr *)&serv_socket, sizeof(serv_socket)); //建立和HTTP服务器的TCP链接
		if (flag < 0)
		{
			continue;
		}
		send_and_recv(sockfd, "www.nynu.edu.cn", "GET", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", ip, port, NULL, "Close"); //下载的主体函数
		close(sockfd);
	}
}


void send_and_recv(int sockfd, char* url, char* fun_type, char* accept_type, char* ip, int port, char* body, char* connection_type)
{
	char* request = (char *) malloc(4 * 1024 * sizeof(char));
	if (body)
		sprintf(request, "%s %s HTTP/1.1\r\nAccept: %s\r\nHost: %s:%d\r\nConnection: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %s\r\n\r\n%d", fun_type, url, accept_type, ip, port, connection_type, body, strlen(body));
	else
		sprintf(request,
			"%s / HTTP/1.1\r\nContent-Type:application/x-www-form-urlencoded\r\nHost:%s\r\nAccept: %s\r\nConnection: %s\r\n\r\n",
			fun_type,
			url,
			accept_type,
			connection_type);
//以上是在组织请求的头部，打印出的结果就是文章开头所写

	
	 send(sockfd, request, strlen(request), MSG_NOSIGNAL);
	//printf("%s", request);
	free(request);
	char* response = (char *) malloc(1024 * sizeof(char));
	int length;
	do
	{
		length = recv(sockfd, response, 1024,MSG_NOSIGNAL);
		char* loc = strstr(response, "\r\n\r\n"); //截获返回头部，以\r\n\r\n为标识
		if (loc)
		{
			int loci = loc - response + 4;
			//write(1, response, loci) ;//如果是响应头部就打印至屏幕
			//write(1, loc, length - loci);
		}
		else
		{
			//write(1, response, length);
		}
		if (length==0||length==-1)//注意，因为之前采用的是close方法，也就是说一旦传输数据完毕，则服务器端会断开链接，则read函数会返回0，所以这里 会退出循环。如果采用的是Keep-Alive则服务器不关闭TCP链接，也就说程序将会被阻塞在read函数中，因此要注意的是自己判断是否读到了响应 的结尾，然后在再次调用read之前退出循环。
			break ;
	} while (1);
	free(response);
}

