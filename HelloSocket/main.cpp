#define  WIN32_LEAN_AND_MEAN //�����window�������ڵĿ⣬�Լ������������
#include <windows.h> //windowAPI
#include <WinSock2.h>//SocketAPI

//���뾲̬���ӿ�
//#pragma comment(lib,"ws2_32.lib")  //����֧��windows


int main()
{
	//�汾��
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	//����WindowsSocket����
	WSAStartup(ver, &dat);

	//�ر�WindowsSocket����(��startup��ƥ��)
	WSACleanup();
	return 0;
}