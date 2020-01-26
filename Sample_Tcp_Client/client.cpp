#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
using namespace std;

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
		else if ( 0 == strcmp(_cmdMsg,"login"))
		{
			Login  _lgin;
			strcpy_s(_lgin.userName, "asus0420");
			strcpy_s(_lgin.passWord, "mw970420");
			//发送数据
			send(_sock, (const char *)&_lgin, sizeof(_lgin), 0);
			//接受服务端返回的数据
			LoginResult _lgRes = {};
			recv(_sock, (char *)&_lgRes, sizeof(_lgRes), 0);
			printf("接收到的数据:%d \n", _lgRes.result);

		}
		else if (0 == strcmp(_cmdMsg,"logout"))
		{
			LogOut _lgout;
			strcpy_s(_lgout.userName, "asus0420");
			//发送数据
			send(_sock, (const char *)&_lgout, sizeof(_lgout), 0);

			//接受服务器的返回的数据
			LogOutResult _lgoutRes = {};
			recv(_sock, (char *)&_lgoutRes, sizeof(_lgoutRes), 0);
			printf("接收到的数据:%d  \n", _lgoutRes.result);
		}
		else
		{
			printf("该命令不支持，请重新输入...\n");
		}
	}

	
	//4.closesocket
	closesocket(_sock);
	WSACleanup();
	printf("客户端退出\n");
	getchar();
	return 0;
}