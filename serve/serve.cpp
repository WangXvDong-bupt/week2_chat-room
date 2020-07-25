// CMake_client.cpp: 定义应用程序的入口点。
//
#include "CMake_client.h"
#include<iostream>
#include<stack>
#include<vector>
#include<cstdio>
#include<string>
#include <process.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;
//using std::cin; using std::cout; using std::endl;

#define maxSize 1024


DWORD WINAPI CreateSendMegThread(LPVOID lpParameter)
{
    SOCKET s_send = (SOCKET)lpParameter;
    while (true)
    {
        int send_len = 0;
        cout << endl << "请输入发送信息:";
        string line;
        getline(cin, line);
        send_len = send(s_send, line.c_str(), maxSize, 0);
        if (send_len == SOCKET_ERROR) {
            cout << "发送失败！" << endl;
            continue;
        }

    }
    return 0;
}

DWORD WINAPI CreateRecvMegThread(LPVOID lpParameter)
{
    SOCKET s_recv = (SOCKET)lpParameter;
    while (true)
    {
        char meg_recv[maxSize] = {};
        int recv_len = 0;
        recv_len = recv(s_recv, meg_recv, maxSize, 0);
        if (recv_len == SOCKET_ERROR) {
            cout << "接受失败！" << endl;
            continue;
        }
        else {
            cout << endl << "服务端信息:" << meg_recv << endl;
        }
    }
    return 0;
}

int main()
{
    //初始化套接字库
    WORD w_req = MAKEWORD(2, 2);//版本号
    WSADATA wsadata;
    //int err;
    //err = WSAStartup(w_req, &wsadata);
    if (WSAStartup(w_req, &wsadata) != 0) {
        cout << "初始化套接字库失败！" << endl;
    }
    else {
        cout << "初始化套接字库成功！" << endl;
    }



    //定义服务端套接字
    SOCKET socketServer;
    //服务端地址
    SOCKADDR_IN server_addr;
    //填充服务端信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(1234);
    //创建套接字
    socketServer = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(socketServer, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
        cout << "服务器连接失败！" << endl;
        WSACleanup();
    }
    else {
        cout << "服务器连接成功！" << endl;
    }

    while (true)
    {
        cout << "请输入用户名" << endl;
        char username[20] = {};
        cin.getline(username, 20);
        cout << "请输入密码" << endl;
        char password[20] = {};
        cin.getline(password, 20);
        if (strcmp(username, "ww") == 0 && strcmp(password, "12") == 0)
        {
            cout << "登录成功" << endl;
            break;
        }
        else if (strcmp(username, "xx") == 0 && strcmp(password, "34") == 0)
        {
            cout << "登录成功" << endl;
            break;
        }
        else
        {
            cout << "登录失败" << endl;
        }
    }
    CreateThread(nullptr, 0, CreateRecvMegThread, (LPVOID)socketServer, 0, nullptr);
    CreateThread(nullptr, 0, CreateSendMegThread, (LPVOID)socketServer, 0, nullptr);
    //_beginthread(recvMeg, 0, &socketServer);
    //_beginthread(sendMeg, 0, &socketServer);
    while (true) {}


    closesocket(socketServer);
    WSACleanup();

    return 0;
}
