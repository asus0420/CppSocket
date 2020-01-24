#define  WIN32_LEAN_AND_MEAN //会避免window引用早期的库，以及依赖库的引用
#include <windows.h> //windowAPI
#include <WinSock2.h>//SocketAPI

//加入静态链接库
//#pragma comment(lib,"ws2_32.lib")  //仅仅支持windows


int main()
{
	//版本号
	WORD ver = MAKEWORD(2, 2);
	WSADATA dat;
	//开启WindowsSocket环境
	WSAStartup(ver, &dat);

	//关闭WindowsSocket环境(与startup相匹配)
	WSACleanup();
	return 0;
}