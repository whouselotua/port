//#define _CRT_SECURE_NO_WARNINGS //안전함수 미사용

#include <winsock2.h> 
#include <stdio.h> 
#include <Ws2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")

#include <windows.h>
#include <process.h> 

#define BUFSIZE 512 

unsigned WINAPI HandleClnt(void* arg);
unsigned WINAPI serveMsg(void* arg);


//컴파일설정링커추가 -lws2_32 
int main()
{
	char myaddr[] = "127.0.0.1";
	WSADATA wsaData;
	SOCKADDR_IN servAddr, clientAddr;
	SOCKET hservSock, hclientSock;
	int szClntAddr; //주소정보길이   

	unsigned short* port = (unsigned short*)malloc(sizeof(short));
	printf("Enter PORT number (5500) :");
	scanf_s("%hu", port);

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("error\r\n");
		return 0;
	}

	hservSock = socket(AF_INET, SOCK_STREAM, 0); //소켓생성  
	if (hservSock == INVALID_SOCKET)
		return 1;

	memset(&servAddr, 0, sizeof(servAddr)); //구조체초기화   
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(*port);
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY); //주소자동할당  
	//servAddr.sin_addr.s_addr = inet_addr(myaddr) ; //로컬호스트사용 

	if (bind(hservSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		printf("Bind Error\n");

	if (listen(hservSock, 5) == SOCKET_ERROR)
		printf("listen Error\n");

	printf("Success complite !!\n");
	printf("listen ok port %u\n", *port);

	////////////////////////////////////////////////////////////////

	HANDLE hThread[2];
	//DWORD wr;
	while (1)
	{
		szClntAddr = sizeof(clientAddr);
		hclientSock = accept(hservSock, (SOCKADDR*)&clientAddr, &szClntAddr);

		hThread[0] =
			(HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hclientSock, 0, NULL);
		hThread[1] =
			(HANDLE)_beginthreadex(NULL, 0, serveMsg, (void*)&hclientSock, 0, NULL);

		/*	if((wr=WaitForSingleObject(hThread[0], INFINITE))==WAIT_FAILED)
			{
			puts("thread wait error");
			return -1;
			}
		*/
		//	WaitForSingleObject(hThread[0], INFINITE);
		//	WaitForSingleObject(hThread[1], INFINITE);
		WaitForMultipleObjects(1, hThread, TRUE, INFINITE);

		//printf("wait result: %s \n", (wr == WAIT_OBJECT_0) ? "signaled" : "time-out");
		///////////////////////////////////////////////////////////////////////////////		
		printf("leave client. [ exit(EXIT)] is complite disconnet \n");

		CloseHandle(hThread[0]);
		CloseHandle(hThread[1]);
	}

	closesocket(hservSock);
	WSACleanup();
	return 0;
}


unsigned WINAPI HandleClnt(void* arg)
{
	SOCKET hclientSock = *((SOCKET*)arg);
	int strLen;
	char test_msg[BUFSIZE];

	while ((strLen = recv(hclientSock, test_msg, sizeof(test_msg), 0)) != 0)
	{
		test_msg[strLen] = 0;
		printf("[%d][recv] %s", strLen, test_msg);

	}
	closesocket(hclientSock);
	return 0;
}

unsigned WINAPI serveMsg(void* arg)
{
	SOCKET hclientSock = *((SOCKET*)arg);
	char nameMsg[BUFSIZE];
	char test_msg[BUFSIZE];

	while (1)
	{
		fgets(test_msg, BUFSIZE, stdin);
		if (!strcmp(test_msg, "exit\n") || !strcmp(test_msg, "EXIT\n"))
		{
			send(hclientSock, "goodbye(exit)", 13, 0);
			shutdown(hclientSock, SD_SEND);
			return 0;
		}
		sprintf_s(nameMsg, "%s", test_msg);
		send(hclientSock, nameMsg, strlen(nameMsg), 0);
	}

	return 0;
}

