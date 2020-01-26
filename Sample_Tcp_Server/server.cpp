#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include<stdio.h>

//命令枚举
enum Operate
{
	Server_Login,
	Server_Login_Result,
	Server_Logout,
	Server_Logout_Result,
	Server_Error,
};

struct DataHeader
{
	int headerLen;//包头：数据长度
	int operate; //操作
};
//登录成功
struct Login :public DataHeader
{
	Login()
	{
		headerLen = sizeof(Login);
		operate = Server_Login;
	}
	char userName[32];
	char passWord[32];
};
struct LoginResult :public DataHeader
{
	LoginResult()
	{
		headerLen = sizeof(LoginResult);
		operate = Server_Login_Result;
		result = 0;
	}
	int result;
};
//登录失败
struct LogOut :public DataHeader
{
	LogOut()
	{
		headerLen = sizeof(LogOut);
		operate = Server_Logout;
	}
	char userName[32];
};
struct LogOutResult :public DataHeader
{
	LogOutResult()
	{
		headerLen = sizeof(LogOutResult);
		operate = Server_Logout_Result;
		result = 0;
	}
	int result;
};
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
	_accSock = accept(_sock, (sockaddr *)&_csin, &_csockaddrlen);
	if (INVALID_SOCKET == _accSock)
	{
		printf("错误，接收到无效客户端\n");
	}
	printf("新的客户端加入 IP = %s\n", inet_ntoa(_csin.sin_addr));

	//循环接受客户端的消息
	while (true)
	{
		DataHeader header = {};
		int _res = recv(_accSock, (char *)&header, sizeof(DataHeader), 0);
		if (_res <= 0)
		{
			printf("客户端已退出，任务结束\n");
			break;
		}
		switch (header.operate)
		{
			//登录命令
		case Server_Login:
		{
							 Login _lgin = {};
							 recv(_accSock, (char *)&_lgin + sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
							 //先省略密码判断的过程
							 LoginResult _inRes;
							 send(_accSock, (const char *)&_inRes, sizeof(LoginResult), 0);
							 printf("收到命令：登录 , 命令长度：%d  ,用户名:%s ,密码:%s \n", _lgin.headerLen,_lgin.userName,_lgin.passWord);
		}
		
			break;
		case  Server_Logout:
		{
							   LogOut 	_lgout = {};
							   recv(_accSock, (char *)&_lgout + sizeof(DataHeader), sizeof(LogOut)-sizeof(DataHeader), 0);
							   LogOutResult _outRes;
							   send(_accSock, (const char *)&_outRes, sizeof(LogOutResult), 0);
							   printf("收到命令：登出 , 命令长度：%d  ,用户名:%s \n", _lgout.headerLen, _lgout.userName);
		}
			break;
		default:
		{
				   header.operate = Server_Error;
				   header.headerLen = 0;
				   send(_accSock, (const char *)&header, sizeof(DataHeader), 0);
		}
			break;
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