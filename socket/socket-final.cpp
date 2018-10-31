// socket-S.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h> // 必须在windwos.h之前
#include <Windows.h>
#include <iostream>
#include <direct.h> //_mkdir函数的头文件
#include <io.h>     //_access函数的头文件

#pragma comment(lib, "ws2_32.lib") 
using namespace std;

char path[1024] = { 0 };//全局唯一变量path

void U2G(char *name,char *buff)//UTF-8转GB2312
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
	ret = recv(socket, rec, 4, 0);//文件名长度
	length = atoi(rec);
	printf("filename length = %d\n", length);
	if (ret <= 0)
	{
		if (WSAEWOULDBLOCK == WSAGetLastError())
		{
			return -1;                                  //需要等待 --》直接continue
		}
		else
		{
			printf("recv filename length error = %d \n", WSAGetLastError());
			return 0;                                   //文件名出现传输（长度）错误 --》直接break
		}
	}
	if (length == 0)
	{
		return -1;                                      //长度为零说明文件名发送完毕，但还是要接收文件内容 --》直接continue	
	}
	else if (length < 0)                                     //必须确保发送长度时都是大于零的，小于零当作错误
	{
		return 0;                                       //文件名出现传输（长度）错误 --》直接break
	}
	ret = my_recv(buff, length, socket);
	if (ret == 0)
	{
		return 0;                                       //文件名出现传输（内容）错误 --》直接break
	}
	else
	{
		return 1;                                       //说明本次接收完毕--》进行拼接后continue
	}
}


int my_folder(char *buff, int socket)//文件夹
{
	int ret = 0;
	int length = 0;
	char recvbuff[1024] = { 0 };
	char rec[8] = { 0 };
	ret = recv(socket, rec, 4, 0);//文件名长度
	length = atoi(rec);
	printf("filename length = %d\n", length);
	if (ret <= 0)
	{
		if (WSAEWOULDBLOCK == WSAGetLastError())
		{
			return -1;                                  //需要等待 --》直接continue
		}
		else
		{
			printf("recv filename length error = %d \n", WSAGetLastError());
			return 0;                                   //文件名出现传输（长度）错误 --》直接break
		}
	}
	if (length == 0)
	{
		return -1;                                      //长度为零说明文件名发送完毕，但还是要接收文件内容 --》直接continue	
	}
	else if (length < 0)                                     //必须确保发送长度时都是大于零的，小于零当作错误
	{
		return 0;                                       //文件名出现传输（长度）错误 --》直接break
	}
	ret = my_recv(buff, length, socket);
	if (ret == 0)
	{
		return 0;                                       //文件名出现传输（内容）错误 --》直接break
	}
	else
	{
		return 1;                                       //说明本次接收完毕--》进行拼接后continue
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
			return 1;                      //返回1说明还需要等待 --》continue
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
		return -1;                      //长度为0，说明发送完毕--》break            //对方发送完毕时正常情况下会关闭socket
	}
	else if (length < 0)                //必须确保发送长度时都是大于零的，小于零当作错误
	{
		printf("recv file fail, length < 0 \n");
		//fclose(fp);
		return -1;                      //长度小于0，发送出错 --》break
	}
	ret = my_recv(recvbuff, length, socket);//接收包内容
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

void CreateDir(const char *dir)//创建目录
{
	int m = 0, n;
	string str1, str2;

	str1 = dir;
	str2 = str1.substr(0, 2);//截取前两个
	str1 = str1.substr(3, str1.size());//截取剩下的

	while (m >= 0)//_mkdir一次只能创建一层目录，所以需要循环截取其中的\\进行创建
	{
		m = str1.find('\\');

		str2 += '\\' + str1.substr(0, m);
		n = _access(str2.c_str(), 0); //判断该目录是否存在
		if (n == -1)
		{
			_mkdir(str2.c_str());     //创建目录
		}

		str1 = str1.substr(m + 1, str1.size());
	}
}



DWORD WINAPI thread(LPVOID connfd)
{
	SOCKET sockFD = (SOCKET)*(SOCKET *)connfd;//这句怎么理解，(SOCKET *)是强制转换成指针，（SOCKET)*呢？
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
					continue;//等待信号
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
					continue;//接文件名下一个的包
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
					continue;//成功，继续接收下一个包
				}
			}
			else if (signal == 2)
			{
				ret = my_folder(recvbuff, sockFD);
				if (ret == -1)
				{
					continue;//等待信号
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
					continue;//接文件名下一个的包
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
	//隐藏窗口
	//HWND   hWnd = ::FindWindow(L"ConsoleWindowClass", NULL);
	//ShowWindow(hWnd, 0);
	//初始化
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("WSAStartup failed!\n");
		system("pause");
		WSACleanup();
		return -1;
	}
	printf("初始化\n");//固定初始化

	//创建socket
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
	printf("创建\n");

	//server_addr初始化
	int port;
	
	//从文件读路径和端口
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

	//绑定
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
	printf("绑定\n");

	//监听
	ret = listen(listenfd, 1);
	if (ret == SOCKET_ERROR)
	{
		printf("listen failed!\n");
		system("pause");
		closesocket(listenfd);
		WSACleanup();
		return -1;
	}
	printf("监听\n");

	//accept、recv
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
	//收尾
	WSACleanup();
	return 0;
}