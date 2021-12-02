#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <mutex>
#include <thread>
#include <list>
#include<functional>
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
void send_func(char* msg, int len, int index);
std::mutex hmutex;
std::list<s_connection> client_list;
std::list<s_connection>::iterator iter;
// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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
    // ���� �ʱ�ȭ
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

    // ������ ��ſ� ����� ����
    SOCKET client_sock;
    Connection* tmp;
    SOCKADDR_IN clientaddr;
    int addrlen;
    char buf[BUFSIZE];
    HANDLE hThread;

    while (1) {
        // accept()
        printf("before accept\n");
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            printf("err\n");
            err_display("accept()");
            break;
        }

        hmutex.lock();
        retval = recv(client_sock, buf, BUFSIZE, 0);//Ŭ���̾�Ʈ�� ����,����,�Ǹ�,mbti�� �޾ƿ�
        buf[retval] = '\0';//�׿� �����͸� ����
        tmp = (Connection*)buf;//����ü ����
        tmp->sock = client_sock;
        tmp->index = client_count++;
        client_list.push_back(*tmp);
        iter = client_list.begin();
        std::advance(iter, tmp->index);
        hmutex.unlock();

        // ������ Ŭ���̾�Ʈ ���� ���
        printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        // ������ ����
        hThread = CreateThread(NULL, 0, recvhandle, &(*iter), 0, NULL);
        CloseHandle(hThread);
    }

    // closesocket()
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}

DWORD WINAPI recvhandle(LPVOID arg) {
    iter = client_list.begin();
    Connection *client = (Connection*)arg;
    std::advance(iter, client->index);
    int len, cycle;
    char msg[BUFSIZE];
    //getpeername(client->sock, (SOCKADDR*)&clientaddr, &addrlen);
    while ((len = recv(client->sock, msg, BUFSIZE, 0)) != 0)
    {
        msg[len] = '\0';
        printf("%d client attempt to send msg...\n",client->index);
        send_func(msg, sizeof(msg), client->index);
    }
    closesocket(client->sock);

    hmutex.lock();
    iter = client_list.begin();
    std::advance(iter, client->index);
    cycle = client->index+1;
    for (iter ; iter != client_list.end(); iter++) {
        if (iter->index > client->index) {
            iter->index--;
        }
        if (cycle++ >= client_count)break;
    }
    iter = client_list.begin();
    std::advance(iter, client->index);
    client_list.erase(iter);
    client_count--;
    hmutex.unlock();


    return 0;
}

void send_func(char* msg, int len, int index) {
    iter = client_list.begin();
    int i, cycle;
    char tmp[512];
    
    hmutex.lock();
    cycle = 1;
    std::advance(iter, index);
    sprintf(tmp, "[%s]: %s", iter->nick, msg);
    for (iter = client_list.begin(); iter != client_list.end(); iter++) {
        send(iter->sock, tmp, sizeof(tmp), 0);
        if (cycle++ >= client_count) break;
    }
    hmutex.unlock();
}