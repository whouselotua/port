#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <mutex>
#include <thread>
#include <list>
#define SERVERPORT 9000
#define BUFSIZE    512


int client_count = 0;
typedef struct s_connection {
    SOCKET sock;
    int index;
    int age;
    char mbti[4];
    char rname[32];
    char nick[32];
}Connection;
DWORD WINAPI recvhandle(LPVOID arg);
void send_func(char* msg, int len,Connection* client);
std::mutex hmutex;

Connection *client_socks[16];
Connection trash;
// 소켓 함수 오류 출력 후 종료
void err_quit(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCWSTR)msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// 소켓 함수 오류 출력
void err_display(const char* msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    printf("[%s] %s", msg, (char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
}


int main(int argc, char* argv[])
{
    int retval;
    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;
    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");
    BOOL enable = TRUE;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char*)&enable, sizeof(enable));
    // bind()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // 데이터 통신에 사용할 변수
    SOCKET client_sock;
    Connection *tmp; 
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE];
    HANDLE hThread;

    while (1) {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            err_display("accept()");
            break;
        }
        hmutex.lock();
        retval=recv(client_sock, buf, BUFSIZE, 0);//클라이언트의 소켓,나이,실명,mbti를 받아옴
        buf[retval] = '\0';//잉여 데이터를 제거
        tmp = (Connection*)buf;//구조체 대입
        tmp->sock = client_sock;//구조체의 소켓을 통신용 소켓으로 바꿈
        tmp->index = client_count;
        client_socks[client_count++] = tmp;
        hmutex.unlock();
        // 접속한 클라이언트 정보 출력
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        // 스레드 생성
        hThread = CreateThread(NULL, 0, recvhandle, client_socks[tmp->index], 0, NULL);
        CloseHandle(hThread);
    }

    // closesocket()
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}

DWORD WINAPI recvhandle(LPVOID arg) {
    Connection *client = (Connection*)arg;
    Connection *tmp;
    int len, addrlen;
    char msg[BUFSIZE];
    SOCKADDR_IN clientaddr;
    addrlen = sizeof(clientaddr);
    //getpeername(client->sock, (SOCKADDR*)&clientaddr, &addrlen);
    while ((len = recv(client->sock, msg, BUFSIZE, 0)) != 0)
    {
        
        msg[len] = '\0';
        send_func(msg, sizeof(msg),client_socks[client->index]);
    }

    hmutex.lock();
    closesocket(client->sock);
   for (int i = client->index; i < client_count-1; i++) {
       client_socks[i] = client_socks[i + 1];
       client_socks[i]->index--;
    }
    client_count--;
    client_socks[client_count] = 0;
    hmutex.unlock();
  
    
    return 0;
}

void send_func(char* msg, int len,Connection* client) {
    int i,retval;
    char tmp[512];
    hmutex.lock();
    for (i = 0; i < client_count; i++) {
        sprintf(tmp, "[%s]: %s", client->nick, msg);
        send(client_socks[i]->sock, tmp, len, 0);
    }
    hmutex.unlock();
}