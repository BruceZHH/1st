// socket-C.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <winsock2.h>
#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <sys/types.h> 
#include <Windows.h>  
#include <iostream>
#include <string.h>
#pragma comment(lib,"Ws2_32.lib") 
using namespace std;//ע����仰��string��---Ū��


int _tmain(int argc, _TCHAR *argv[])
{
	char sendbuff[1024] = { 0 };//�����Ƿ�����Ż���������
	char info[1024] = { 0 };
	int n = 0;
	char ip[16] = { 0 };
	char filename[1024] ;
	char path[1024];
	int signal ;//0--�ļ���   1--�ļ�����
	int sendifo = 0;
	int length = 0;
	char sendnum[4] = { 0 };
	int socketfd;
	int port ;

	FILE *fp = fopen("config.ini", "r");
	fscanf(fp, "port=%d,info=%s",&port,info);
	printf("port = %d\n", port);
	printf("info = %s\n", info);
	fclose(fp);
	//�ָ�info,�ο����ܶ�CreateDir()
	int m,a = 2;
	string str1, str2;
	str1 = info;
	while (a >= 0)//�Ƿ���Ҫд��ѭ����
	{
	m = str1.find(',');
	str2 =str1.substr(0, m);//��ȡ������ȡ���������⣡����������malloc����path/filename/ip�� 
	if (a == 2)
		memcpy(path, str2.c_str(), 1024);//strlenӦ���ǱȽϺõ�ȡ����ʽ�����ǣ��ڷ��ͳ���ֵ���Լ�����Ŀ���ʱ���������������ÿռ���������⣬��1024�Ͳ���������
	if (a == 1)
		memcpy(filename, str2.c_str(), 1024);
	if (a == 0)
		memcpy(ip, str2.c_str(), 16);
	str1 = str1.substr(m + 1, str1.size());//��������
	a--;
	}
	//��һ�䣿������Ҫ��Ҫ�ӣ�   path[strlen(path) + 1] = '\0';
	printf("path = %s\n", path);
	printf("filename = %s\n", filename);
	printf("ip = %s\n", ip);
	system("pause");
	fp = fopen(filename, "r+ ");


	//��ʼ��������Ҫ������ע���ͷš�����ʼ������socket�ᱨ��getlasterror = 10093
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup failed\n");
		system("pause");
		return -1;
	}

	//socket()�����׽���

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd != INVALID_SOCKET)
		printf("socket success\n");

	//	�ͻ���û��Ҫ�������ģʽ

	//connect()���ӷ�����
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip);//thinkpad:192.168.191.2    dell:192.168.103.11
    sendifo = connect(socketfd, (sockaddr *)&server_addr, sizeof(sockaddr));
	if (sendifo == SOCKET_ERROR)
	{
		printf("connect failed, error = %d\n", WSAGetLastError());
		WSACleanup();
		closesocket(socketfd);//WSACleanuph��closesocket�Ⱥ�����
		system("pause");
		return -1;
	}
	else
	{
		printf("socket connect success\n");
	}



	//�����ļ�·��
	signal = 2;
	itoa(signal, sendnum, 10);
	sendifo = send(socketfd, sendnum, 1, 0);
	printf("sig = %s\n", sendnum);
	if (sendifo == SOCKET_ERROR)
	{
		printf("filename signal send error, error = %d \n", WSAGetLastError());
		system("pause");
		return -1;
	}
	else
	{
		length = strlen(path);
		itoa(length, sendnum, 10);
		sendifo = send(socketfd, sendnum, 4, 0);
		printf("length = %d,sendnum = %s\n", length, sendnum);

		sendifo = send(socketfd, path, strlen(path), 0);
		printf("path = %s,size = %d\n", path, strlen(path));
		if (sendifo == SOCKET_ERROR)
		{
			printf("filename send error, error = %d \n", WSAGetLastError());
			system("pause");
			return -1;
		}
	}


	//�����ļ���
	signal = 0 ;
	itoa(signal,sendnum,10);
	sendifo = send(socketfd, sendnum, 1, 0);
	printf("sig = %s\n",sendnum);
	if (sendifo == SOCKET_ERROR)
	{
		printf("filename signal send error, error = %d \n",WSAGetLastError());
		system("pause");
		return -1;
	}
	else
	{
		length = strlen(filename);
		itoa(length,sendnum,10);
		sendifo = send(socketfd, sendnum, 4, 0);
		printf("length = %d,sendnum = %s\n", length, sendnum);

		sendifo = send(socketfd, filename, strlen(filename), 0);
		printf("filename = %s,size = %d\n",filename,strlen(filename));
		if (sendifo == SOCKET_ERROR)
		{
			printf("filename send error, error = %d \n",WSAGetLastError());
			system("pause");
			return -1;
		}
	}

	//�����ļ�����
	while (true)
	{
		signal = 1;
		itoa(signal, sendnum, 10);
		sendifo = send(socketfd, sendnum, 1, 0);
		printf("sig = %s\n", sendnum);
		if (sendifo == SOCKET_ERROR)
		{
			printf("file signal send error, error = %d \n", WSAGetLastError());
			system("pause");
			break;
		}

		n = fread(sendbuff, 1, sizeof(sendbuff), fp);
		printf("sendbuff = %s\n",sendbuff);
		if (n > 0)
		{
			printf("n = %d\n", n);
			itoa(n, sendnum, 10);
			sendifo = send(socketfd, sendnum, 4, 0);
			printf("sendifo = %d,  sendnum = %s\n",sendifo, sendnum);

			sendifo = send(socketfd, sendbuff, n, 0);
			memset(sendbuff, 0, sizeof(sendbuff));
			printf("sendifo = %d\n", sendifo);
			if (SOCKET_ERROR == sendifo)
				{
					printf("file send failed, error = %d\n", WSAGetLastError());
					break;
				}

		}
		if (n == 0)//�Ѿ�����
		{
			itoa(n, sendnum, 10);
			sendifo = send(socketfd, sendnum, 4, 0);
			printf("n = %d\n", n);
			break;
		}

	}

	system("pause");

	//��β����
	fclose(fp);
	WSACleanup();
	closesocket(socketfd);
	return 0;
}

