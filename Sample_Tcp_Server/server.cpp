#define  WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>
#include<stdio.h>

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
	char  msg[] = "��˵ɶ�ţ�\n";
	_accSock = accept(_sock, (sockaddr *)&_csin, &_csockaddrlen);
	if (INVALID_SOCKET == _accSock)
	{
		printf("���󣬽��յ���Ч�ͻ���\n");
	}
	printf("�µĿͻ��˼��� IP = %s\n", inet_ntoa(_csin.sin_addr));
	char _revMsg[128] = {};
	//ѭ�����ܿͻ��˵���Ϣ
	while (true)
	{
		int _res = recv(_accSock, _revMsg, 128, 0);
		if (_res <= 0)
		{
			printf("�ͻ������˳����������\n");
			break;
		}
		if ( 0 == strcmp(_revMsg,"getName"))
		{
			printf("���յ�����:getName\n");
			send(_accSock, "Ĳ��", 20, 0);
		}
		else if (0 == strcmp(_revMsg,"getAge"))
		{
			printf("���յ�����:getAge\n");
			send(_accSock, "23", 20, 0);
		}
		else
		{
			//5.send ��ͻ��˷���һ������
			//char  msg[] = "Hello , I'm Server.";
			printf("���յ�����:%s\n",_revMsg);
			send(_accSock, msg, strlen(msg) + 1, 0);
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