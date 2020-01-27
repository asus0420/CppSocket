#define  WIN32_LEAN_AND_MEAN
#define  NO_RECEIVE -1

#include <windows.h>
#include <WinSock2.h>
#include <iostream>
using namespace std;

//����ö��
enum Operate
{
	Server_Login,
	Server_Login_Result,
	Server_Logout,
	Server_Logout_Result,
	Server_New_User_Join,
	Server_Error,
};

struct DataHeader
{
	int headerLen;//��ͷ�����ݳ���
	int operate; //����
};
//��¼�ɹ�
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
//��¼ʧ��
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
	char recvbuffer [1024] = { };
	int _res_recv = recv ( _sock , recvbuffer , sizeof( DataHeader ) , 0 );
	DataHeader* header = ( DataHeader* ) recvbuffer;
	if ( _res_recv <= 0 )
	{
		cout << "������δ��Ӧ" << endl;
		return NO_RECEIVE;
	}
	switch ( header->operate )
	{
		case Server_Login_Result:
		{
									recv ( _sock , recvbuffer + sizeof( DataHeader ) , header->headerLen - sizeof( DataHeader ) , 0 );
									LoginResult	  *_inRes = ( LoginResult * ) recvbuffer;
									cout << "�յ�������Ӧ��Server_Login_Result" << "Ӧ�𳤶�Ϊ��" << _inRes->headerLen << endl;
		}
			break;
		case Server_Logout_Result:
		{
									 recv ( _sock , recvbuffer + sizeof( DataHeader ) , header->headerLen - sizeof( DataHeader ) , 0 );
									 LogOutResult	  *_outRes = ( LogOutResult * ) recvbuffer;
									 cout << "�յ�������Ӧ��Server_Logout_Result" << "Ӧ�𳤶�Ϊ��" << _outRes->headerLen <<endl;
		}
			break;
		case Server_New_User_Join:
		{
									 recv ( _sock , recvbuffer + sizeof( DataHeader ) , header->headerLen - sizeof( DataHeader ) , 0 );
									 NewUerJoin	  *_nuj = ( NewUerJoin * ) recvbuffer;
									 //cout << "�յ�������Ӧ��Server_New_User_Join" << "Ӧ�𳤶�Ϊ��" << _nuj->headerLen << endl;
									 cout << _nuj->sock << "�����˷�����" << endl;
		}
			break;
	}
	return 1;
}


int main()
{
	//creatre version number
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	//����
	WSAStartup(ver, &data);
	//1.socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == _sock)
	{
		printf("�����׽���ʧ��\n");
	}else{
		printf("�����׽��ֳɹ�\n");
	}
	//2.connect
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(4567); // host to net unsigned short
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int cnt = connect(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in));
	if (SOCKET_ERROR == cnt)
	{
		printf("����ʧ��\n");
	}
	else{
		printf("���ӳɹ�\n");
	}

	while (true)
	{
		fd_set _fds_read;
		FD_ZERO ( &_fds_read );
		FD_SET ( _sock , &_fds_read );
		int _res_select  = select ( _sock , &_fds_read , nullptr , nullptr , nullptr );
		if (_res_select < 0)
		{
			printf ( "select�����Ѿ�����\n" );
			break;
		}
		if ( FD_ISSET ( _sock , &_fds_read ) )
		{
			FD_CLR ( _sock , &_fds_read );
			if ( NO_RECEIVE == process_handle ( _sock ) )
			{
					  break;
			}
		}
		
	}

	
	//4.closesocket
	closesocket(_sock);
	WSACleanup();
	printf("�ͻ����˳�\n");
	getchar();
	return 0;
}



/*
char _cmdMsg[128] = {};
cin >> _cmdMsg;
if (0 == strcmp(_cmdMsg,"exit"))
{
printf("�ͻ���ִ��exit,�ͻ����˳�\n");
break;
}
else if ( 0 == strcmp(_cmdMsg,"login"))
{
Login  _lgin;
strcpy_s(_lgin.userName, "asus0420");
strcpy_s(_lgin.passWord, "mw970420");
//��������
send(_sock, (const char *)&_lgin, sizeof(_lgin), 0);
//���ܷ���˷��ص�����
LoginResult _lgRes = {};
recv(_sock, (char *)&_lgRes, sizeof(_lgRes), 0);
printf("���յ�������:%d \n", _lgRes.result);

}
else if (0 == strcmp(_cmdMsg,"logout"))
{
LogOut _lgout;
strcpy_s(_lgout.userName, "asus0420");
//��������
send(_sock, (const char *)&_lgout, sizeof(_lgout), 0);

//���ܷ������ķ��ص�����
LogOutResult _lgoutRes = {};
recv(_sock, (char *)&_lgoutRes, sizeof(_lgoutRes), 0);
printf("���յ�������:%d  \n", _lgoutRes.result);
}
else
{
printf("�����֧�֣�����������...\n");
}
*/