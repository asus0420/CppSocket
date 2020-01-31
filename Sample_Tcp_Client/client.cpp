#define NO_RECEIVE                  (-1)

#ifdef _WIN32
	#define  WIN32_LEAN_AND_MEAN
	#define WINSOCK_DEPRECATED_NO_WARNING
	#include <windows.h>
	#include <WinSock2.h>
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>

#define  SOCKET				int
#define  SOCKET_ERROR						   (-1)
#define  INVALID_SOCKET   SOCKET(~0)
#endif

#include<iostream>
#include <thread>
using namespace std;

//枚举--命令
enum Operate
{
	Server_Login ,
	Server_Login_Result ,
	Server_Logout ,
	Server_Logout_Result ,
	Server_New_User_Join ,
	Server_Error ,
};
//数据头
struct DataHeader
{
	int headerLen;//包头：数据长度
	int operate; //操作
};
//登录
struct Login :public DataHeader
{
	Login ()
	{
		headerLen = sizeof( Login );
		operate = Server_Login;
	}
	char userName [32];
	char passWord [32];
};
//登录结果
struct LoginResult :public DataHeader
{
	LoginResult ()
	{
		headerLen = sizeof( LoginResult );
		operate = Server_Login_Result;
		result = 0;
	}
	int result;
};
//登出
struct LogOut :public DataHeader
{
	LogOut ()
	{
		headerLen = sizeof( LogOut );
		operate = Server_Logout;
	}
	char userName [32];
};
//登出结果
struct LogOutResult :public DataHeader
{
	LogOutResult ()
	{
		headerLen = sizeof( LogOutResult );
		operate = Server_Logout_Result;
		result = 0;
	}
	int result;
};
//新用户加入
struct NewUerJoin :public DataHeader
{
	NewUerJoin ()
	{
		headerLen = sizeof( NewUerJoin );
		operate = Server_New_User_Join;
		sock = 0;
	}
	int sock;
};


int process_handle ( SOCKET _sock )
{
	char recvBuf [1024];
	int res_recv = (int)recv ( _sock , recvBuf , sizeof( DataHeader ) , 0 );
	if (res_recv <= 0)
	{
		cout << "服务器未响应" << endl;
		return NO_RECEIVE;
	}
	DataHeader * header = ( DataHeader* ) recvBuf;
	switch ( header->operate )
	{
		case Server_Login_Result:
		{
									recv ( _sock , recvBuf + sizeof( DataHeader ) , header->headerLen - sizeof( DataHeader ) , 0 );
									LoginResult * login = ( LoginResult * ) recvBuf;
									cout << "收到命令：Server_Login_Result，数据长度：" << login->headerLen << endl;
		}
			break;
		case Server_Logout_Result:
		{
									 recv ( _sock , recvBuf + sizeof( DataHeader ) , header->headerLen - sizeof( DataHeader ) , 0 );
									 LogOutResult * logout = ( LogOutResult * ) recvBuf;
									 cout << "收到命令：Server_Logout_Result，数据长度：" << logout->headerLen << endl;
		}
			break;
		case Server_New_User_Join:
		{
									 recv ( _sock , recvBuf + sizeof( DataHeader ) , header->headerLen - sizeof( DataHeader ) , 0 );
									 NewUerJoin *nuj = ( NewUerJoin * ) recvBuf;
									 cout << "收到命令：Server_New_User_Join，数据长度：" << nuj->headerLen << endl;
		}
			break;
	}
	return 0;
}

bool is_run_client = true;
void thread_command ( SOCKET _sock )
{
	while (true)
	{
		char sendMsg [256] = { };
		cin >> sendMsg;
		if ( 0 == strcmp ( sendMsg , "exit" ) )
		{
			is_run_client = false;
			cout << "线程退出，任务结束" << endl;
			  break;
		}
		else  if (0 == strcmp(sendMsg,"login"))
		{
			Login login = { };
#ifdef _WIN32
			strcpy_s ( login.userName , "moonlink" );
			strcpy_s(login.passWord,"www.moonlink.club");
#else
			strcpy ( login.userName , "moonlink" );
			strcpy ( login.passWord , "www.moonlink.club" );
#endif 
			send ( _sock , ( const char * ) &login , sizeof( login ) , 0 );
			cout << "发送登陆命令" << endl;
		}
		else if (0 == strcmp(sendMsg,"logout"))
		{
			LogOut logout;
#ifdef _WIN32
			strcpy_s ( logout.userName , "moonlink" );
#else
			strcpy ( logout.userName , "moonlink" );
#endif
			send ( _sock , ( const char * ) &logout , sizeof( logout ) , 0 );
			cout << "发送登出命令" << endl;
		}
		else
		{
			cout << "命令输入有误，请重新输入" << endl;
		}
	}
	
}


int main ()
{
#ifdef _WIN32
	WORD ver = MAKEWORD ( 2 , 2 );
	WSADATA dat;
	WSAStartup ( ver , &dat );
#endif
	//1.socket 创建socket
	SOCKET sock = socket ( AF_INET , SOCK_STREAM , 0 );
	if (INVALID_SOCKET == sock)
	{
		cout << "套接字创建失败..." << endl;
	}
	else
	{
		cout << "套接字创建成功..." << endl;
	}
	//2.connect
	sockaddr_in s_addr_in = { };
	s_addr_in.sin_family = AF_INET;
	s_addr_in.sin_port = htons ( 4567 );
#ifdef _WIN32
	s_addr_in.sin_addr.S_un.S_addr = inet_addr ( "127.0.0.1" );
#else
	s_addr_in.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
	int res_con = (int)connect ( sock , ( const sockaddr * )& s_addr_in , sizeof( s_addr_in ) );
	if ( SOCKET_ERROR == res_con )
	{
		cout << "连接失败..." << endl;
	}
	else
	{
		cout << "连接成功..." << endl;
	}
	thread th (thread_command,sock);
	th.detach ();
	while (is_run_client)
	{
		fd_set fds_read;
		FD_ZERO ( &fds_read );
		FD_SET ( sock , &fds_read );
		timeval time_clock { 1 , 0 };
		int res_sel = ( int ) select ( sock , &fds_read , nullptr , nullptr , &time_clock );
		if (res_sel < 0)
		{
			cout << "糟糕，select出错" << endl;
			break;
		}
		if (FD_ISSET(sock,&fds_read))
		{
			FD_CLR ( sock , &fds_read );
			if (NO_RECEIVE == process_handle(sock))
			{
				cout << "糟糕，select出错1" << endl;
				break;
			}
		}
		//cout << "客户端执行其他的任务" << endl;
	}

#ifdef _WIN32
	closesocket ( sock );
	WSACleanup ();
#else
	close(sock);
#endif
	return 0;
}
