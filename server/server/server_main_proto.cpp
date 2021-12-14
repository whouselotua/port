#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
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

bool game_on = false;
char* mbti;
int client_count = 0;
typedef struct s_connection {
    SOCKET sock;
    int index;
    int score;
    char age[32];
    char mbti[32];
    char rname[32];
    char nick[32];

}Connection;
DWORD WINAPI recvhandle(LPVOID arg);
void send_func(char* msg, int len, int index);
void opcode_func(Connection* arg);
std::mutex hmutex;
std::list<s_connection> client_list;
std::list<s_connection>::iterator iter;
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
        retval = recv(client_sock, buf, BUFSIZE, 0);//클라이언트의 소켓,나이,실명,mbti를 받아옴
        buf[retval] = '\0';//잉여 데이터를 제거
        tmp = (Connection*)buf;//구조체 대입
        tmp->sock = client_sock;
        tmp->index = client_count++;
        client_list.push_back(*tmp);
        iter = client_list.begin();
        std::advance(iter, tmp->index);
        hmutex.unlock();

        // 접속한 클라이언트 정보 출력
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        // 스레드 생성
        hThread = CreateThread(NULL, 0, recvhandle, &(*iter), 0, NULL);
        CloseHandle(hThread);
    }

    // closesocket()
    printf("close listen_sock");
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}

DWORD WINAPI recvhandle(LPVOID arg) {
    Connection *client = (Connection*)arg;
    int len, tmp, cmp=0, cycle=1;
    char msg[BUFSIZE];
    char whisper[BUFSIZE];
    //getpeername(client->sock, (SOCKADDR*)&clientaddr, &addrlen);
    sprintf(msg, "\n%s 클라이언트가 접속하였습니다.\n", client->nick);
    for (iter = client_list.begin(); iter != client_list.end(); iter++) {
        if (iter->index == client->index) continue;
        send(iter->sock, msg, sizeof(msg), 0);
        if (cycle++ >= client_count) break;
    }
    while ((len = recv(client->sock, msg, BUFSIZE, 0)) != 0)
    {
        if (len == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        msg[len] = '\0';
        if ((game_on == true) && !(strcmp(msg, mbti)))
        {
            hmutex.lock();
            game_on = false;
            client->score++;
            hmutex.unlock();
            sprintf(msg, "%s가 점수를 획득하였습니다!", client->nick);
            cycle = 1;
            for (iter = client_list.begin(); iter != client_list.end(); iter++) {
                //if (iter->index == index) continue;
                send(iter->sock, msg, sizeof(msg), 0);
                if (cycle++ >= client_count) break;
            }
            continue;
        }
            if ((strcmp(msg,"opcode\0")==0) || (strcmp(msg,"Opcode\0")==0) || (strcmp(msg,"OPCODE\0")==0)) {
                opcode_func(client);
                continue;
            }
        send_func(msg, sizeof(msg), client->index);
    }
    cycle = 1;
    sprintf(msg, "%s 클라이언트가 나갔습니다.\n", client->nick);
    for (iter = client_list.begin(); iter != client_list.end(); iter++) {
        if (iter->index == client->index) continue;
        send(iter->sock, msg, sizeof(msg), 0);
        if (cycle++ >= client_count) break;
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
    if (iter->score >= 10) {
        sprintf(tmp, "%s [%s]: %s", "'너와 나의 눈높이다'", iter->nick, msg);
    }
    else if (iter->score > 6) {
        sprintf(tmp, "%s [%s]: %s", "'이 몸,등장'", iter->nick, msg);
    }
    else if (iter->score > 3) {
        sprintf(tmp, "%s [%s]: %s", "'내가 누구?'", iter->nick, msg);
    }
    else {
        sprintf(tmp, "%s [%s]: %s", "'응애'", iter->nick, msg);
    }
    //sprintf(tmp, "[%s]: %s", iter->nick, msg);
    for (iter = client_list.begin(); iter != client_list.end(); iter++) {
        if (iter->index == index) continue;
        send(iter->sock, tmp, sizeof(tmp), 0);
        if (cycle++ >= client_count) break;
    }
    hmutex.unlock();
}

void opcode_func(Connection* arg) {
    Connection* client = arg;
    int tmp, cycle, len, cmp=0;
    char op_msg[BUFSIZE];
    char whisper[BUFSIZE];
    strcpy(op_msg, "\n어떤 기능을 이용 하시겠습니까?\n 1:귓속말 | 2: 프로필 보기 | 3: MBTI맞추기\n");
    send(client->sock, op_msg, sizeof(op_msg), 0);
    recv(client->sock, op_msg, BUFSIZE, 0);
    tmp = atoi(op_msg);
    switch (tmp) {
    case 1:
        cycle = 1;
        strcpy(op_msg, "누구에게 보내시겠습니까?\n");
        for (iter = client_list.begin(); iter != client_list.end(); iter++) {
            sprintf(whisper, "%s\n", iter->nick);
            strcat(op_msg, whisper);
            if (cycle++ >= client_count) break;
        }
        send(client->sock, op_msg, sizeof(op_msg), 0);
        len = recv(client->sock, op_msg, BUFSIZE, 0);
        op_msg[len] = '\0';
        cmp = 0;
        cycle = 1;
        for (iter = client_list.begin(); iter != client_list.end(); iter++) {
            if (!strcmp(op_msg, iter->nick)) {
                break;
            }
            else cmp++;
            if (cycle++ >= client_count) break;
        }
        if (cmp == client_count) {
            strcpy(op_msg, "해당 클라이언트가 없습니다.");
            send(client->sock, op_msg, sizeof(op_msg), 0);
            break;
        }
        iter = client_list.begin();
        std::advance(iter, cmp);
        strcpy(op_msg, "보낼 내용을 입력해주세요\n");
        send(client->sock, op_msg, sizeof(op_msg), 0);
        memset(op_msg, 0, BUFSIZE);
        recv(client->sock, op_msg, BUFSIZE, 0);
        sprintf(whisper, "(귓속말)[%s]: %s\n", client->nick, op_msg);
        send(iter->sock, whisper, sizeof(whisper), 0);
        break;
    case 2:
        cycle = 1;
        strcpy(op_msg, "누구의 프로필을 보시겠습니까?\n");
        for (iter = client_list.begin(); iter != client_list.end(); iter++) {
            sprintf(whisper, "%s\n", iter->nick);
            strcat(op_msg, whisper);
            if (cycle++ >= client_count) break;
        }
        send(client->sock, op_msg, sizeof(op_msg), 0);
        len = recv(client->sock, op_msg, BUFSIZE, 0);
        op_msg[len] = '\0';
        cmp = 0;
        cycle = 1;
        for (iter = client_list.begin(); iter != client_list.end(); iter++) {
            if (!strcmp(op_msg, iter->nick)) {
                break;
            }
            else cmp++;
            if (cycle++ >= client_count) break;
        }
        if (cmp == client_count) {
            strcpy(op_msg, "해당 클라이언트가 없습니다.");
            send(client->sock, op_msg, sizeof(op_msg), 0);
            break;
        }
        iter = client_list.begin();
        std::advance(iter, cmp);
        sprintf(whisper, "\n---------------%s가 당신의 프로필을 열람하였습니다---------------%\n", client->nick);
        send(iter->sock, whisper, BUFSIZE, 0);
        sprintf(op_msg, "나이:%s\n실명:%s\nMBTI:%s\n", iter->age, iter->rname, iter->mbti);
        send(client->sock, op_msg, sizeof(op_msg), 0);
        break;
        case 3:
            if (game_on == true) {
                strcpy(op_msg, "다른 이의 게임이 끝난 후 시도하십시오.\n");
                send(client->sock, op_msg, sizeof(op_msg), 0);
                break;
            }
            sprintf(op_msg, "%s가 MBTI 게임을 시작하였습니다!\n", client->nick);
            cycle = 1;
            for (iter = client_list.begin(); iter != client_list.end(); iter++) {
                //if (iter->index == index) continue;
                send(iter->sock, op_msg, sizeof(op_msg), 0);
                if (cycle++ >= client_count) break;
            }
            hmutex.lock();
            mbti = client->mbti;
            game_on = true;
            hmutex.unlock();
            break;
        default:
             break;

    }
}