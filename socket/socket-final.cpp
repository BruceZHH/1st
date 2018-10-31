// socket-S.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h> // ������windwos.h֮ǰ
#include <Windows.h>
#include <iostream>
#include <direct.h> //_mkdir������ͷ�ļ�
#include <io.h>     //_access������ͷ�ļ�

#pragma comment(lib, "ws2_32.lib") 
using namespace std;

char path[1024] = { 0 };//ȫ��Ψһ����path

void U2G(char *name,char *buff)//UTF-8תGB2312
{
	int len = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
	wchar_t wstr[1024] = {0};

	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, name, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char str[1024] = {0};
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	strcpy(buff, str);
	//return str;

//	int wLen = MultiByteToWideChar(CP_ACP, 0, name, -1, NULL, 0);
//	LPWSTR wStr = new WCHAR[wLen];
//	MultiByteToWideChar(CP_ACP, 0, name, -1, wStr, wLen);
//
//	int aLen2 = WideCharToMultiByte(CP_ACP, 0, wStr, -1, NULL, 0, NULL, NULL);
//	char* converted2 = new char[aLen2];
//	WideCharToMultiByte(CP_ACP, 0, wStr, -1, converted2, aLen2, NULL, NULL);
}

int my_recv(char *recv_data, int recv_length, int client_socket)
{
	int ret_length = 0;
	int ret_total_length = 0;
	while (1)
	{
		ret_length = recv(client_socket, recv_data + ret_total_length, recv_length - ret_total_length, 0);
		printf("recv length = %d\n", ret_length);
		if (ret_length < 0)
		{
			if (WSAEWOULDBLOCK == WSAGetLastError())
			{
				continue;
			}
			else
			{
				printf("recv  error = %d \n", WSAGetLastError());
				return 0;
			}
		}
		else if (ret_length == 0)
		{
			printf("socket close\n");
			return 0;
		}
		else
		{
			ret_total_length += ret_length;
			if (ret_total_length == recv_length)
			{
				break;
			}
		}
	}
	printf("recv success \n");
	return 1;
}

int my_filename(char *buff, int socket)
{
	int ret = 0;
	int length = 0;
	char recvbuff[1024] = { 0 };//
	char rec[8] = { 0 };
	ret = recv(socket, rec, 4, 0);//�ļ�������
	length = atoi(rec);
	printf("filename length = %d\n", length);
	if (ret <= 0)
	{
		if (WSAEWOULDBLOCK == WSAGetLastError())
		{
			return -1;                                  //��Ҫ�ȴ� --��ֱ��continue
		}
		else
		{
			printf("recv filename length error = %d \n", WSAGetLastError());
			return 0;                                   //�ļ������ִ��䣨���ȣ����� --��ֱ��break
		}
	}
	if (length == 0)
	{
		return -1;                                      //����Ϊ��˵���ļ���������ϣ�������Ҫ�����ļ����� --��ֱ��continue	
	}
	else if (length < 0)                                     //����ȷ�����ͳ���ʱ���Ǵ�����ģ�С���㵱������
	{
		return 0;                                       //�ļ������ִ��䣨���ȣ����� --��ֱ��break
	}
	ret = my_recv(buff, length, socket);
	if (ret == 0)
	{
		return 0;                                       //�ļ������ִ��䣨���ݣ����� --��ֱ��break
	}
	else
	{
		return 1;                                       //˵�����ν������--������ƴ�Ӻ�continue
	}
}


int my_folder(char *buff, int socket)//�ļ���
{
	int ret = 0;
	int length = 0;
	char recvbuff[1024] = { 0 };
	char rec[8] = { 0 };
	ret = recv(socket, rec, 4, 0);//�ļ�������
	length = atoi(rec);
	printf("filename length = %d\n", length);
	if (ret <= 0)
	{
		if (WSAEWOULDBLOCK == WSAGetLastError())
		{
			return -1;                                  //��Ҫ�ȴ� --��ֱ��continue
		}
		else
		{
			printf("recv filename length error = %d \n", WSAGetLastError());
			return 0;                                   //�ļ������ִ��䣨���ȣ����� --��ֱ��break
		}
	}
	if (length == 0)
	{
		return -1;                                      //����Ϊ��˵���ļ���������ϣ�������Ҫ�����ļ����� --��ֱ��continue	
	}
	else if (length < 0)                                     //����ȷ�����ͳ���ʱ���Ǵ�����ģ�С���㵱������
	{
		return 0;                                       //�ļ������ִ��䣨���ȣ����� --��ֱ��break
	}
	ret = my_recv(buff, length, socket);
	if (ret == 0)
	{
		return 0;                                       //�ļ������ִ��䣨���ݣ����� --��ֱ��break
	}
	else
	{
		return 1;                                       //˵�����ν������--������ƴ�Ӻ�continue
	}
}

int my_file(FILE *fp, int socket)
{
	int ret = 0;
	int length = 0;
	char recvbuff[1024] = { 0 };
	char rec[8] = { 0 };
	//FILE *fp = fopen(buff, "ab+");
	ret = recv(socket, rec, 4, 0);
	length = atoi(rec);
	printf("file length = %d\n", length);
	if (ret <= 0)
	{
		if (WSAEWOULDBLOCK == WSAGetLastError())
		{
			//fclose(fp);
			return 1;                      //����1˵������Ҫ�ȴ� --��continue
		}
		else
		{
			printf("recv file length error = %d \n", WSAGetLastError());
			//fclose(fp);
			return -1;
		}
	}
	if (length == 0)
	{
		printf("recv file success \n");
		//fclose(fp);
		return -1;                      //����Ϊ0��˵���������--��break            //�Է��������ʱ��������»�ر�socket
	}
	else if (length < 0)                //����ȷ�����ͳ���ʱ���Ǵ�����ģ�С���㵱������
	{
		printf("recv file fail, length < 0 \n");
		//fclose(fp);
		return -1;                      //����С��0�����ͳ��� --��break
	}
	ret = my_recv(recvbuff, length, socket);//���հ�����
	if (ret == 0)
	{
		printf("recv file error \n");
		//fclose(fp);
		return -1;
	}
	else
	{
		fwrite(recvbuff, length, 1, fp);
		//fclose(fp);
		return 1;
	}
}

void CreateDir(const char *dir)//����Ŀ¼
{
	int m = 0, n;
	string str1, str2;

	str1 = dir;
	str2 = str1.substr(0, 2);//��ȡǰ����
	str1 = str1.substr(3, str1.size());//��ȡʣ�µ�

	while (m >= 0)//_mkdirһ��ֻ�ܴ���һ��Ŀ¼��������Ҫѭ����ȡ���е�\\���д���
	{
		m = str1.find('\\');

		str2 += '\\' + str1.substr(0, m);
		n = _access(str2.c_str(), 0); //�жϸ�Ŀ¼�Ƿ����
		if (n == -1)
		{
			_mkdir(str2.c_str());     //����Ŀ¼
		}

		str1 = str1.substr(m + 1, str1.size());
	}
}



DWORD WINAPI thread(LPVOID connfd)
{
	SOCKET sockFD = (SOCKET)*(SOCKET *)connfd;//�����ô��⣬(SOCKET *)��ǿ��ת����ָ�룬��SOCKET)*�أ�
	FILE *fp = NULL;
	char *recvbuff = (char*)malloc(1024 * sizeof(char));
	char *buff = (char*)malloc(1024 * sizeof(char));	
	char *file_path = (char*)malloc(1024 * sizeof(char));
	
	memset(buff, 0, 1024);
	memset(recvbuff, 0, 1024);
	memset(file_path, 0, 1024);
	//memset(folder_path, 0, 1024);
	strcpy(file_path, path);
	char name[1024] = { 0 };
	char folder_buff[1024] = { 0 };
	int ret = 0;
	int signal = 0;
	char sig[2] = { 0 };
	
	while (true)
	{
		ret = recv(sockFD, sig, 1, 0);
		signal = atoi(sig);
		if (ret <= 0)
		{
			if (WSAEWOULDBLOCK == WSAGetLastError())
			{
				continue;
			}
			else
			{
				printf("recv signal error, error = %d\n", WSAGetLastError);
				break;
			}
		}
		else
		{
			printf("signal = %d\n", signal);
			if (signal == 0)
			{
				ret = my_filename(recvbuff, sockFD);
				if (ret == -1)
				{
					continue;//�ȴ��ź�
				}
				else if (ret == 0)
				{
					break;
				}
				else if (ret == 1)
				{
					strcat(name, recvbuff);
					//name[strlen(recvbuff) + 1] = '\0';
					//recvbuff[1024] = { 0 };
					memset(recvbuff, 0, 1024);
					//memcpy(buff, U2G(name), strlen(U2G(name)));

					U2G(name, buff);
					file_path[strlen(file_path) + 1] = '\0';
					printf("filename = %s\n", file_path);
					memset(buff, 0, 1024);
					fp = fopen(file_path, "wb+");
					continue;//���ļ�����һ���İ�
				}
			}
			else if (signal == 1)
			{
				ret = my_file(fp, sockFD);
				if (ret == -1)
				{
					break;
				}
				else if (ret == 1)
				{
					continue;//�ɹ�������������һ����
				}
			}
			else if (signal == 2)
			{
				ret = my_folder(recvbuff, sockFD);
				if (ret == -1)
				{
					continue;//�ȴ��ź�
				}
				else if (ret == 0)
				{
					break;
				}
				else if (ret == 1)
				{
					strcpy(folder_buff, recvbuff);
					
					//recvbuff[1024] = { 0 };
					memset(recvbuff, 0, 1024);
					//memcpy(buff, U2G(name), strlen(U2G(name)));
					U2G(folder_buff,buff);
					printf("folderpath = %s\n", buff);

					char *token = strtok(buff, "####");

					printf("token = %s\n", token);

					strcat(file_path, token);
					file_path[strlen(file_path) + 1] = '\0';
					
					
					printf("file_path = %s\n", file_path);
					CreateDir(file_path);

					memset(buff, 0, 1024);
					continue;//���ļ�����һ���İ�
				}
			}


		}
	}
	memset(recvbuff, 0, 1024);
	if (recvbuff)
	{
		free(recvbuff);
		recvbuff = NULL;
	}
	
	memset(file_path, 0, 1024);
	if (file_path)
	{
		free(file_path);
		file_path = NULL;
	}
	
	fclose(fp);
	closesocket(sockFD);
	return 0;
}

int _tmain(int argc, _TCHAR *argv[])
{
	//���ش���
	//HWND   hWnd = ::FindWindow(L"ConsoleWindowClass", NULL);
	//ShowWindow(hWnd, 0);
	//��ʼ��
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("WSAStartup failed!\n");
		system("pause");
		WSACleanup();
		return -1;
	}
	printf("��ʼ��\n");//�̶���ʼ��

	//����socket
	SOCKET listenfd;
	SOCKET clientfd;
	listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenfd == INVALID_SOCKET)
	{
		printf("socket failed!\n");
		system("pause");
		WSACleanup();
		return -1;
	}
	printf("����\n");

	//server_addr��ʼ��
	int port;
	
	//���ļ���·���Ͷ˿�
	FILE *fp = fopen("config.ini", "r");
	fscanf(fp, "port=%d,path=%s", &port, path);
	printf("port = %d\n", port);
	//fscanf(fp, "path=%s", path);
	printf("path = %s\n", path);
	fclose(fp);
	path[strlen(path) + 1] = '\0';

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	//��
	int ret;
	ret = bind(listenfd, (sockaddr *)&server_addr, sizeof(SOCKADDR_IN));
	if (ret == SOCKET_ERROR)
	{
		printf("bind failed!\n");
		system("pause");
		closesocket(listenfd);
		WSACleanup();
		return -1;
	}
	printf("��\n");

	//����
	ret = listen(listenfd, 1);
	if (ret == SOCKET_ERROR)
	{
		printf("listen failed!\n");
		system("pause");
		closesocket(listenfd);
		WSACleanup();
		return -1;
	}
	printf("����\n");

	//accept��recv
	while (1)
	{
		clientfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr_in clientaddr;
		int clientaddrlen = sizeof(clientaddr);
		clientfd = accept(listenfd, (sockaddr *)&clientaddr, &clientaddrlen);
		if (clientfd == SOCKET_ERROR)
		{
			int err = WSAGetLastError();
			if (err == WSAEWOULDBLOCK)
			{
				Sleep(100);
				continue;
			}
			else
			{
				printf("accept fail\n");
				Sleep(100);
				continue;
			}
		}
		else
		{
			HANDLE recthread = CreateThread(NULL, 0, thread, &clientfd, 0, 0);
			Sleep(100);
		}
	}
	//��β
	WSACleanup();
	return 0;
}