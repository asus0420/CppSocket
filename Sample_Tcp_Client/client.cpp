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
		char _cmdMsg[128] = {};
		cin >> _cmdMsg;
		if (0 == strcmp(_cmdMsg,"exit"))
		{
			printf("�ͻ���ִ��exit,�ͻ����˳�\n");
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
			printf("���յ����ݣ�%s \n", _tips);
		}
		else
		{
			printf("δ���յ�����\n");
		}
	}

	
	//4.closesocket
	closesocket(_sock);
	WSACleanup();
	printf("�ͻ����˳�\n");
	getchar();
	return 0;
}