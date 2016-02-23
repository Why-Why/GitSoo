#include<iostream>
#include<WinSock2.h>
#include<cstring>
#pragma comment( lib, "ws2_32.lib")
using namespace std;
#include <WS2tcpip.h>

int main()
{
	WORD wVersionRequested;
	int err;
	WSADATA wsadata;

	wVersionRequested = MAKEWORD(1, 1);
	err = WSAStartup(wVersionRequested, &wsadata);
	if (LOBYTE(wsadata.wVersion) != 1 || HIBYTE(wsadata.wVersion) != 1)
	{
		WSACleanup();
		cerr << "Error1";
		return -1;
	}
	if (err)
	{
		cerr << "Error2";
		return -2;
	}


	
		SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);
		SOCKADDR_IN addrClt;
		addrClt.sin_family = AF_INET;
		addrClt.sin_port = htons(12345);
		addrClt.sin_addr.S_un.S_addr = inet_pton(AF_INET, "115.28.136.33", (void*)"115.28.136.33");


		while (SOCKET_ERROR == connect(socketClient, (SOCKADDR*)&addrClt, sizeof(addrClt)))
			cerr << "Error3";
		cerr << "Connected!";

		char sendBuf[50];
		strcat_s("oh God !", 50, sendBuf);
		if (send(socketClient, sendBuf, strlen(sendBuf) + 1, 0) < 0)
			cerr << "Error4";

		char recvBuf[50];
		int count;
		count = recv(socketClient, recvBuf, 50, 0);
		recvBuf[count] = 0;
		cout << recvBuf << endl;

		closesocket(socketClient);
		WSACleanup();


}
