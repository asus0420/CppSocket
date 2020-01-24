#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include<stdio.h>

int main()
{
	//creatre version number
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	//开启
	WSAStartup(ver, &data);
	//--利用Socket API 建立一个简易的服务端
	//1.socket 创建socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET  == _sock)
	{
		printf("创建套接字失败\n");
	}
	else
	{
		printf("创建套接字成功\n");
	}
	//2.bind 绑定一个端口用于socket服务
	sockaddr_in _sin = {}; //初始化结构体
	_sin.sin_family = AF_INET;  //协议族 与 socket保持一致
	_sin.sin_port = htons(4567);  //host to net unsigned int
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1"); //服务器绑定的IP
	if (SOCKET_ERROR == bind(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in))){
		printf("绑定端口失败...\n");
	}else{
		printf("绑定端口成功...\n");
	}


	//3.listen 监听网络端口
	if (SOCKET_ERROR == listen(_sock,5)){
		printf("监听端口失败...\n");
	}else{
		printf("监听端口成功...\n");
	}

	//4.accept 等待客户端连接
	sockaddr_in _csin = {};//初始化，客户端套接字地址
	int _csockaddrlen = sizeof(sockaddr_in);//返回客户端的套接字的长度
	SOCKET _accSock = INVALID_SOCKET; //令为无效套接字
	char  msg[] = "你说啥呐？\n";
	_accSock = accept(_sock, (sockaddr *)&_csin, &_csockaddrlen);
	if (INVALID_SOCKET == _accSock)
	{
		printf("错误，接收到无效客户端\n");
	}
	printf("新的客户端加入 IP = %s\n", inet_ntoa(_csin.sin_addr));
	char _revMsg[128] = {};
	//循环接受客户端的消息
	while (true)
	{
		int _res = recv(_accSock, _revMsg, 128, 0);
		if (_res <= 0)
		{
			printf("客户端已退出，任务结束\n");
			break;
		}
		if ( 0 == strcmp(_revMsg,"getName"))
		{
			printf("接收到命令:getName\n");
			send(_accSock, "牟文", 20, 0);
		}
		else if (0 == strcmp(_revMsg,"getAge"))
		{
			printf("接收到命令:getAge\n");
			send(_accSock, "23", 20, 0);
		}
		else
		{
			//5.send 向客户端发送一条数据
			//char  msg[] = "Hello , I'm Server.";
			printf("接收到命令:%s\n",_revMsg);
			send(_accSock, msg, strlen(msg) + 1, 0);
		}
	}
	

	//6.closesockt 关闭sockt
	closesocket(_sock);



	//关闭
	WSACleanup();

	printf("服务端退出\n");
	getchar();
	return 0;
}