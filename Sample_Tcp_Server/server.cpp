#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include<stdio.h>

//����ö��
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
int main()
{
	//creatre version number
	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	//����
	WSAStartup(ver, &data);
	//--����Socket API ����һ�����׵ķ����
	//1.socket ����socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET  == _sock)
	{
		printf("�����׽���ʧ��\n");
	}
	else
	{
		printf("�����׽��ֳɹ�\n");
	}
	//2.bind ��һ���˿�����socket����
	sockaddr_in _sin = {}; //��ʼ���ṹ��
	_sin.sin_family = AF_INET;  //Э���� �� socket����һ��
	_sin.sin_port = htons(4567);  //host to net unsigned int
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;//inet_addr("127.0.0.1"); //�������󶨵�IP
	if (SOCKET_ERROR == bind(_sock, (sockaddr *)&_sin, sizeof(sockaddr_in))){
		printf("�󶨶˿�ʧ��...\n");
	}else{
		printf("�󶨶˿ڳɹ�...\n");
	}
	//3.listen ��������˿�
	if (SOCKET_ERROR == listen(_sock,5)){
		printf("�����˿�ʧ��...\n");
	}else{
		printf("�����˿ڳɹ�...\n");
	}

	//4.accept �ȴ��ͻ�������
	sockaddr_in _csin = {};//��ʼ�����ͻ����׽��ֵ�ַ
	int _csockaddrlen = sizeof(sockaddr_in);//���ؿͻ��˵��׽��ֵĳ���
	SOCKET _accSock = INVALID_SOCKET; //��Ϊ��Ч�׽���
	_accSock = accept(_sock, (sockaddr *)&_csin, &_csockaddrlen);
	if (INVALID_SOCKET == _accSock)
	{
		printf("���󣬽��յ���Ч�ͻ���\n");
	}
	printf("�µĿͻ��˼��� IP = %s\n", inet_ntoa(_csin.sin_addr));

	//ѭ�����ܿͻ��˵���Ϣ
	while (true)
	{
		DataHeader header = {};
		int _res = recv(_accSock, (char *)&header, sizeof(DataHeader), 0);
		if (_res <= 0)
		{
			printf("�ͻ������˳����������\n");
			break;
		}
		switch (header.operate)
		{
			//��¼����
		case Server_Login:
		{
							 Login _lgin = {};
							 recv(_accSock, (char *)&_lgin + sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
							 //��ʡ�������жϵĹ���
							 LoginResult _inRes;
							 send(_accSock, (const char *)&_inRes, sizeof(LoginResult), 0);
							 printf("�յ������¼ , ����ȣ�%d  ,�û���:%s ,����:%s \n", _lgin.headerLen,_lgin.userName,_lgin.passWord);
		}
		
			break;
		case  Server_Logout:
		{
							   LogOut 	_lgout = {};
							   recv(_accSock, (char *)&_lgout + sizeof(DataHeader), sizeof(LogOut)-sizeof(DataHeader), 0);
							   LogOutResult _outRes;
							   send(_accSock, (const char *)&_outRes, sizeof(LogOutResult), 0);
							   printf("�յ�����ǳ� , ����ȣ�%d  ,�û���:%s \n", _lgout.headerLen, _lgout.userName);
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
	
	//6.closesockt �ر�sockt
	closesocket(_sock);



	//�ر�
	WSACleanup();

	printf("������˳�\n");
	getchar();
	return 0;
}