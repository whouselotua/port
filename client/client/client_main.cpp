#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "3.144.101.142"
#define SERVERPORT 9000
#define BUFSIZE    512
DWORD WINAPI SendMsg(LPVOID arg);
DWORD WINAPI RecvMsg(LPVOID arg);
typedef struct s_connection {
    SOCKET sock;
    int index;
    int score;
    char age[32];
    char mbti[32];
    char rname[32];
    char nick[32];
}Connection;


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

// ����� ���� ������ ���� �Լ�

int main(int argc, char* argv[])
{
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;
    Connection client;
    memset(client.rname, 0, 32);
    Connection* ptr = &client;
    // socket()
    client.sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client.sock == INVALID_SOCKET) err_quit("socket()");
    printf("���̸� �Է��ϼ���: ");
    scanf("%s", client.age);
    client.age[strlen(client.age)] = '\0';
    getchar();
    printf("������ ������ �� ����� ��ó����?\n");
    printf("���� ������ or ������ ������(E/I�� ���)\n");
    printf("�׳� �׷��� ���� or ��� �׷� ���� ����(S/N)\n");
    printf("���ش� �ȵ����� ���� or ���ذ� �ž� ����(F/T)\n");  
    printf("�Ѵٸ� �� or ������ ����(J/P)\n");
    printf("ex)ISFJ\n");
    scanf("%s", client.mbti);
    client.mbti[strlen(client.mbti)] = '\0';
    getchar();
    printf("���� �̸��� �Է��ϼ���: ");
    scanf("%s", client.rname);
    client.rname[strlen(client.rname)] = '\0';
    getchar();
    printf("ä�ÿ� �� �г����� �����ϼ���: ");
    scanf("%s", client.nick);
    client.nick[strlen(client.nick)] = '\0';
    getchar();
    client.score = 0;
    // connect()
    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = connect(client.sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("connect()");
    
    send(client.sock,(char*)&client, sizeof(client), 0);//���� �ȳ�
    // ������ ��ſ� ����� ����

    int len;

    HANDLE Send;
    HANDLE Recv;
    Send = CreateThread(NULL, 0, SendMsg, ptr, 0, NULL);
    Recv = CreateThread(NULL, 0, RecvMsg, ptr, 0, NULL);
    WaitForSingleObject(Send, INFINITE);
    WaitForSingleObject(Recv, INFINITE);
    CloseHandle(Send);
    CloseHandle(Recv);

    // closesocket()
    closesocket(client.sock);

    // ���� ����
    WSACleanup();
    return 0;
}

DWORD WINAPI SendMsg(LPVOID arg) {
    Connection* client = (Connection*)arg;
    int retval = 0;
    int len;
    char send_buf[BUFSIZE + 1];
    printf("�����Ӱ� ä���� ġ�� �˴ϴ�.(����� exit | EXIT�� �Է��ϼ���.)\n opcode | OPCODE | Opcode �� �Է½� �ӼӸ�/������ ����/MBTI������ ���� �� �ֽ��ϴ�.\n");
    while (1) {
        
        fgets(send_buf, BUFSIZE+1, stdin);
        if (!strcmp(send_buf, "exit\n") || !strcmp(send_buf, "EXIT\n"))
        {
            shutdown(client->sock, SD_SEND);
            break;
        }
        len = strlen(send_buf);
        if (send_buf[len - 1] == '\n')
            send_buf[len - 1] = '\0';
        if (strlen(send_buf) == 0) {
            continue;
        }
        retval=send(client->sock, send_buf, strlen(send_buf), 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }
    }
    return 0;
}

DWORD WINAPI RecvMsg(LPVOID arg)
{
    Connection *client = (Connection*)arg;
    int len;
    char tmp[BUFSIZE + 1];
    char recv_buf[BUFSIZE + 1];

    while ((len = recv(client->sock, recv_buf, BUFSIZE, 0)) != 0)
    {
        if (len == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        if (len == -1)
            return -1;
        recv_buf[len] = '\0';
        printf("\n");
        fputs(recv_buf, stdout);
        printf("\n");
    }
    return 0;
}