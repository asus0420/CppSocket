#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
using namespace std;


int main()
{
	//creatre version number
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	//开启
	WSAStartup(ver, &data);
	//1.socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		printf("创建套接字失败\n");
	}else{
		printf("创建套接字成功\n");
	}
	//2.connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); // host to net unsigned short
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int cnt = connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == cnt)
	{
		printf("连接失败\n");
	}
	else{
		printf("连接成功\n");
	}

	while (true)
	{
		char _cmdMsg[128] = {};
		cin >> _cmdMsg;
		if (0 == strcmp(_cmdMsg,"exit"))
		{
			printf("客户端执行exit,客户端退出\n");
			break; 
		}
		else
		{
			send(_sock, _cmdMsg, strlen(_cmdMsg) + 1, 0);
		}

		//3.recv
		char _tips[128] = {};
		int rev = recv(_sock, _tips, 128, 0);
		if (rev > 0)
		{
			printf("接收到数据：%s \n", _tips);
		}
		else
		{
			printf("未接收到数据\n");
		}
	}

	
	//4.closesocket
	closesocket(_sock);
	WSACleanup();
	printf("客户端退出\n");
	getchar();
	return 0;
}