#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE    512
char buf[BUFSIZE + 1];
DWORD WINAPI SendMsg(LPVOID arg);
DWORD WINAPI RecvMsg(LPVOID arg);
typedef struct s_connection {
    SOCKET sock;
    int index;
    int age;
    int score;
    char mbti[5];
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
int recvn(SOCKET s, char* buf, int len, int flags)
{
    int received;
    char* ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
}

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
    scanf("%d", &client.age);
    getchar();
    printf("������ ������ �� ����� ��ó����?\n");
    printf("���� ������ or ������ ������(E/I�� ���)\n");
    printf("�׳� �׷��� ���� or ��� �׷� ���� ����(S/N)\n");
    printf("���ش� �ȵ����� ���� or ���ذ� �ž� ����(F/T)\n: ");
    printf("�Ѵٸ� �� or ������ ����(J/P)\n");
    scanf("%s", client.mbti);
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
    char name_buf[BUFSIZE + 20];
    int retval = 0;
    printf("�����Ӱ� ä���� ġ�� �˴ϴ�.(����� exit | EXIT�� �Է��ϼ���.)\n opcode | OPCODE | Opcode �� �Է½� �ӼӸ�/������ ����/MBTI������ ���� �� �ֽ��ϴ�.\n");
    while (1) {
        
        fgets(buf, BUFSIZE, stdin);
        if (!strcmp(buf, "exit\n") || !strcmp(buf, "EXIT\n"))
        {
            shutdown(client->sock, SD_SEND);
            exit(0);
        }
        buf[strlen(buf) - 1] = '\0';
        retval=send(client->sock, buf, strlen(buf), 0);
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

    while ((len = recv(client->sock, buf, sizeof(buf), 0)) != 0)
    {
        if (len == -1)
            return -1;
        buf[len] = 0;
        fputs(buf, stdout);
        printf("\n");
    }
    return 0;
}