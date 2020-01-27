#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

//未接受到信息
#define  NO_RECEIVE -1

//命令枚举
enum Operate
{
	Server_Login ,
	Server_Login_Result ,
	Server_Logout ,
	Server_Logout_Result ,
	Server_New_User_Join,
	Server_Error ,
};

struct DataHeader
{
	int headerLen;//包头：数据长度
	int operate; //操作
};
//登录成功
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
//登录失败
struct LogOut :public DataHeader
{
	LogOut ()
	{
		headerLen = sizeof( LogOut );
		operate = Server_Logout;
	}
	char userName [32];
};
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
struct NewUerJoin:public DataHeader
{
	NewUerJoin ()
	{
		headerLen = sizeof( NewUerJoin );
		operate = Server_New_User_Join;
		sock = 0;
	}
	int sock;
};

std::vector<SOCKET>  vec_client_sock;
///=============================================================
///函数名:	process_handle
///作用: 响应客户端的命令
///
int process_handle ( SOCKET _accSock )
{
	//缓冲区
	char headbuffer [1024] = { };
	int _res = recv ( _accSock , headbuffer , sizeof( DataHeader ) , 0 );
	DataHeader *header = ( DataHeader* ) headbuffer;
	if ( _res <= 0 )
	{
		printf ( "客户端已退出，任务结束\n" );
		return NO_RECEIVE;
	}
	switch ( header->operate )
	{
		//登录命令
		case Server_Login:
		{
							 recv ( _accSock , headbuffer + sizeof( DataHeader ) , header->headerLen - sizeof(DataHeader) , 0 );
							 Login * _lgin = ( Login * ) headbuffer;
							 //先省略密码判断的过程
							 LoginResult _inRes;
							 send ( _accSock , ( const char * ) &_inRes , sizeof( LoginResult ) , 0 );
							 printf ( "收到命令：登录 , 命令长度：%d  ,用户名:%s ,密码:%s \n" , _lgin->headerLen , _lgin->userName , _lgin->passWord );
		}

			break;
		case  Server_Logout:
		{
							   recv ( _accSock , headbuffer + sizeof( DataHeader ) , header->headerLen - sizeof( DataHeader ) , 0 );
							   LogOut *	_lgout = ( LogOut * ) headbuffer;
							   LogOutResult _outRes;
							   send ( _accSock , ( const char * ) &_outRes , sizeof( LogOutResult ) , 0 );
							   printf ( "收到命令：登出 , 命令长度：%d  ,用户名:%s \n" , _lgout->headerLen , _lgout->userName );
		}
			break;
		default:
		{
				   header->operate = Server_Error;
				   header->headerLen = 0;
				   send ( _accSock , ( const char * ) &header , sizeof( DataHeader ) , 0 );
		}
			break;
	}
}
int main ()
{
	//creatre version number
	WORD ver = MAKEWORD ( 2 , 2 );
	WSADATA data;
	//开启
	WSAStartup ( ver , &data );
	//--利用Socket API 建立一个简易的服务端
	//1.socket 创建socket
	SOCKET _sock = socket ( AF_INET , SOCK_STREAM , IPPROTO_TCP );
	if ( INVALID_SOCKET == _sock )
	{
		printf ( "创建套接字失败\n" );
	} else
	{
		printf ( "创建套接字成功\n" );
	}
	//2.bind 绑定一个端口用于socket服务
	sockaddr_in _sin = { }; //初始化结构体
	_sin.sin_family = AF_INET;  //协议族 与 socket保持一致
	_sin.sin_port = htons ( 4567 );  //host to net unsigned int
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1"); //服务器绑定的IP
	if ( SOCKET_ERROR == bind ( _sock , ( sockaddr * ) &_sin , sizeof( sockaddr_in ) ) )
	{
		printf ( "绑定端口失败...\n" );
	} else
	{
		printf ( "绑定端口成功...\n" );
	}
	//3.listen 监听网络端口
	if ( SOCKET_ERROR == listen ( _sock , 5 ) )
	{
		printf ( "监听端口失败...\n" );
	} else
	{
		printf ( "监听端口成功...\n" );
	}

	//循环接受客户端的消息
	while ( true )
	{
		//伯克利SocketAPI
		fd_set _fds_read;
		fd_set _fds_write;
		fd_set _fds_except;
		//清空
		FD_ZERO ( &_fds_read );
		FD_ZERO ( &_fds_write );
		FD_ZERO ( &_fds_except );
		//赋值
		FD_SET ( _sock , &_fds_read );
		FD_SET ( _sock , &_fds_write );
		FD_SET ( _sock , &_fds_except );

		for ( int n = ( int ) vec_client_sock.size () - 1; n >= 0; --n )
		{
			FD_SET ( vec_client_sock [n] , &_fds_read );
		}
		//Select模型
		/*
			@pram1:nfds			   类型：整数，含义：fd_set中所有描述符(socket)的范围，而不是数量 (windows中此参数无意义,可以写0)；
			@pram2:readfds   （可选）指针，指向一组等待可读性检查的套接口；
			@pram3:writefds  （可选）指针，指向一组等待可写性检查的套接口；
			@pram4:exceptfds（可选）指针，指向一组等待错误检查的套接口；
			@pram5: timeout     本函数最多等待时间，对阻塞操作则为NULL。
			*/
		timeval _time_clock = { 0 , 0 };
		int _res_select = select ( _sock + 1 , &_fds_read , &_fds_write , &_fds_except , &_time_clock );
		//如果选择的返回小于0
		if ( _res_select < 0 )
		{
			printf ( "select任务结束\n" );
			break;
		}
		if ( FD_ISSET ( _sock , &_fds_read ) )
		{
			//重要，先要清空_fds_read里的内容
			FD_CLR ( _sock , &_fds_read );
			//4.accept 等待客户端连接
			sockaddr_in _csin = { };//初始化，客户端套接字地址
			int _csockaddrlen = sizeof( sockaddr_in );//返回客户端的套接字的长度
			SOCKET _accSock = INVALID_SOCKET; //令为无效套接字
			_accSock = accept ( _sock , ( sockaddr * ) &_csin , &_csockaddrlen );
			if ( INVALID_SOCKET == _accSock )
			{
				printf ( "错误，接收到无效客户端\n" );
			}
			//向加入到服务器中的其他客户端发送新客户端加入
			for ( int i = 0; i < ( int ) vec_client_sock.size (); ++i )
			{
				NewUerJoin _nuj;
				_nuj.sock = vec_client_sock [i];
				send ( vec_client_sock [i] , ( const char * ) &_nuj , sizeof( NewUerJoin ) , 0 );
			}
			//添加到动态数组中
			vec_client_sock.push_back ( _accSock );
			printf ( "新的客户端加入Socket = %d ,  IP = %s\n" , _accSock , inet_ntoa ( _csin.sin_addr ) );
		}
		for ( size_t n = 0; n < _fds_read.fd_count; ++n )
		{
			//如果该客户端退出，则将该客户端的套接字移除
			if ( NO_RECEIVE == process_handle ( _fds_read.fd_array [n] ) )
			{
				auto iter = find ( vec_client_sock.begin () , vec_client_sock.end () , _fds_read.fd_array [n] );
				//如果此迭代器不等于最后一个的后一个，则移除此元素
				if ( iter != vec_client_sock.end () )
				{
					vec_client_sock.erase ( iter );
				}
			}
		}
	}

	//如果select函数返回值小于0，则关闭所有的socket
	for ( size_t n = 0; n < vec_client_sock.size (); ++n )
	{
		printf ( "关闭第%d个套接字" , n );
		closesocket ( vec_client_sock [n] );
	}

	//6.closesockt 关闭sockt
	closesocket ( _sock );



	//关闭
	WSACleanup ();

	printf ( "服务端退出\n" );
	getchar ();
	return 0;
}