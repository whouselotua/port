#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
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

// 사용자 정의 데이터 수신 함수

int main(int argc, char* argv[])
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;
    Connection client;
    memset(client.rname, 0, 32);
    Connection* ptr = &client;
    // socket()
    client.sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client.sock == INVALID_SOCKET) err_quit("socket()");
    printf("나이를 입력하세요: ");
    scanf("%s", client.age);
    client.age[strlen(client.age)] = '\0';
    getchar();
    printf("문제가 생겼을 때 당신의 대처법은?\n");
    printf("말이 많아짐 or 생각이 많아짐(E/I로 대답)\n");
    printf("그냥 그런가 보다 or 어떻게 그럴 수가 있지(S/N)\n");
    printf("이해는 안되지만 공감 or 이해가 돼야 공감(F/T)\n");  
    printf("한다면 함 or 뭐부터 하지(J/P)\n");
    printf("ex)ISFJ\n");
    scanf("%s", client.mbti);
    client.mbti[strlen(client.mbti)] = '\0';
    getchar();
    printf("실제 이름을 입력하세요: ");
    scanf("%s", client.rname);
    client.rname[strlen(client.rname)] = '\0';
    getchar();
    printf("채팅에 쓸 닉네임을 설정하세요: ");
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
    
    send(client.sock,(char*)&client, sizeof(client), 0);//오류 안남
    // 데이터 통신에 사용할 변수

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

    // 윈속 종료
    WSACleanup();
    return 0;
}

DWORD WINAPI SendMsg(LPVOID arg) {
    Connection* client = (Connection*)arg;
    int retval = 0;
    int len;
    char send_buf[BUFSIZE + 1];
    printf("자유롭게 채팅을 치면 됩니다.(종료시 exit | EXIT를 입력하세요.)\n opcode | OPCODE | Opcode 를 입력시 귓속말/프로필 열람/MBTI게임을 고르실 수 있습니다.\n");
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