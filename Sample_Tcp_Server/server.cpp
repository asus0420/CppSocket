#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include<stdio.h>
#include <iostream>
#include <vector>

using namespace std;

//δ���ܵ���Ϣ
#define  NO_RECEIVE -1

//����ö��
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
	int headerLen;//��ͷ�����ݳ���
	int operate; //����
};
//��¼�ɹ�
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
//��¼ʧ��
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
///������:	process_handle
///����: ��Ӧ�ͻ��˵�����
///
int process_handle ( SOCKET _accSock )
{
	//������
	char headbuffer [1024] = { };
	int _res = recv ( _accSock , headbuffer , sizeof( DataHeader ) , 0 );
	DataHeader *header = ( DataHeader* ) headbuffer;
	if ( _res <= 0 )
	{
		printf ( "�ͻ������˳����������\n" );
		return NO_RECEIVE;
	}
	switch ( header->operate )
	{
		//��¼����
		case Server_Login:
		{
							 recv ( _accSock , headbuffer + sizeof( DataHeader ) , header->headerLen - sizeof(DataHeader) , 0 );
							 Login * _lgin = ( Login * ) headbuffer;
							 //��ʡ�������жϵĹ���
							 LoginResult _inRes;
							 send ( _accSock , ( const char * ) &_inRes , sizeof( LoginResult ) , 0 );
							 printf ( "�յ������¼ , ����ȣ�%d  ,�û���:%s ,����:%s \n" , _lgin->headerLen , _lgin->userName , _lgin->passWord );
		}

			break;
		case  Server_Logout:
		{
							   recv ( _accSock , headbuffer + sizeof( DataHeader ) , header->headerLen - sizeof( DataHeader ) , 0 );
							   LogOut *	_lgout = ( LogOut * ) headbuffer;
							   LogOutResult _outRes;
							   send ( _accSock , ( const char * ) &_outRes , sizeof( LogOutResult ) , 0 );
							   printf ( "�յ�����ǳ� , ����ȣ�%d  ,�û���:%s \n" , _lgout->headerLen , _lgout->userName );
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
	//����
	WSAStartup ( ver , &data );
	//--����Socket API ����һ�����׵ķ����
	//1.socket ����socket
	SOCKET _sock = socket ( AF_INET , SOCK_STREAM , IPPROTO_TCP );
	if ( INVALID_SOCKET == _sock )
	{
		printf ( "�����׽���ʧ��\n" );
	} else
	{
		printf ( "�����׽��ֳɹ�\n" );
	}
	//2.bind ��һ���˿�����socket����
	sockaddr_in _sin = { }; //��ʼ���ṹ��
	_sin.sin_family = AF_INET;  //Э���� �� socket����һ��
	_sin.sin_port = htons ( 4567 );  //host to net unsigned int
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1"); //�������󶨵�IP
	if ( SOCKET_ERROR == bind ( _sock , ( sockaddr * ) &_sin , sizeof( sockaddr_in ) ) )
	{
		printf ( "�󶨶˿�ʧ��...\n" );
	} else
	{
		printf ( "�󶨶˿ڳɹ�...\n" );
	}
	//3.listen ��������˿�
	if ( SOCKET_ERROR == listen ( _sock , 5 ) )
	{
		printf ( "�����˿�ʧ��...\n" );
	} else
	{
		printf ( "�����˿ڳɹ�...\n" );
	}

	//ѭ�����ܿͻ��˵���Ϣ
	while ( true )
	{
		//������SocketAPI
		fd_set _fds_read;
		fd_set _fds_write;
		fd_set _fds_except;
		//���
		FD_ZERO ( &_fds_read );
		FD_ZERO ( &_fds_write );
		FD_ZERO ( &_fds_except );
		//��ֵ
		FD_SET ( _sock , &_fds_read );
		FD_SET ( _sock , &_fds_write );
		FD_SET ( _sock , &_fds_except );

		for ( int n = ( int ) vec_client_sock.size () - 1; n >= 0; --n )
		{
			FD_SET ( vec_client_sock [n] , &_fds_read );
		}
		//Selectģ��
		/*
			@pram1:nfds			   ���ͣ����������壺fd_set������������(socket)�ķ�Χ������������ (windows�д˲���������,����д0)��
			@pram2:readfds   ����ѡ��ָ�룬ָ��һ��ȴ��ɶ��Լ����׽ӿڣ�
			@pram3:writefds  ����ѡ��ָ�룬ָ��һ��ȴ���д�Լ����׽ӿڣ�
			@pram4:exceptfds����ѡ��ָ�룬ָ��һ��ȴ���������׽ӿڣ�
			@pram5: timeout     ���������ȴ�ʱ�䣬������������ΪNULL��
			*/
		timeval _time_clock = { 0 , 0 };
		int _res_select = select ( _sock + 1 , &_fds_read , &_fds_write , &_fds_except , &_time_clock );
		//���ѡ��ķ���С��0
		if ( _res_select < 0 )
		{
			printf ( "select�������\n" );
			break;
		}
		if ( FD_ISSET ( _sock , &_fds_read ) )
		{
			//��Ҫ����Ҫ���_fds_read�������
			FD_CLR ( _sock , &_fds_read );
			//4.accept �ȴ��ͻ�������
			sockaddr_in _csin = { };//��ʼ�����ͻ����׽��ֵ�ַ
			int _csockaddrlen = sizeof( sockaddr_in );//���ؿͻ��˵��׽��ֵĳ���
			SOCKET _accSock = INVALID_SOCKET; //��Ϊ��Ч�׽���
			_accSock = accept ( _sock , ( sockaddr * ) &_csin , &_csockaddrlen );
			if ( INVALID_SOCKET == _accSock )
			{
				printf ( "���󣬽��յ���Ч�ͻ���\n" );
			}
			//����뵽�������е������ͻ��˷����¿ͻ��˼���
			for ( int i = 0; i < ( int ) vec_client_sock.size (); ++i )
			{
				NewUerJoin _nuj;
				_nuj.sock = vec_client_sock [i];
				send ( vec_client_sock [i] , ( const char * ) &_nuj , sizeof( NewUerJoin ) , 0 );
			}
			//��ӵ���̬������
			vec_client_sock.push_back ( _accSock );
			printf ( "�µĿͻ��˼���Socket = %d ,  IP = %s\n" , _accSock , inet_ntoa ( _csin.sin_addr ) );
		}
		for ( size_t n = 0; n < _fds_read.fd_count; ++n )
		{
			//����ÿͻ����˳����򽫸ÿͻ��˵��׽����Ƴ�
			if ( NO_RECEIVE == process_handle ( _fds_read.fd_array [n] ) )
			{
				auto iter = find ( vec_client_sock.begin () , vec_client_sock.end () , _fds_read.fd_array [n] );
				//����˵��������������һ���ĺ�һ�������Ƴ���Ԫ��
				if ( iter != vec_client_sock.end () )
				{
					vec_client_sock.erase ( iter );
				}
			}
		}
	}

	//���select��������ֵС��0����ر����е�socket
	for ( size_t n = 0; n < vec_client_sock.size (); ++n )
	{
		printf ( "�رյ�%d���׽���" , n );
		closesocket ( vec_client_sock [n] );
	}

	//6.closesockt �ر�sockt
	closesocket ( _sock );



	//�ر�
	WSACleanup ();

	printf ( "������˳�\n" );
	getchar ();
	return 0;
}