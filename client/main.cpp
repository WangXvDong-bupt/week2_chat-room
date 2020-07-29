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
#define maxSize 1024

//定义储存用户ID的数据类型
typedef struct userInformition
{
    string username;
    string password;
}node_user;
//定义向量用于存储用户ID信息
vector<node_user> user_login = { {"ww","12"},{"xx","34"} };

string Login(const vector<node_user> &user_login)
{
    string username;
    string password;
    while (true)
    {
        cout << "请输入用户名" << endl;
        cin >> username;
        if (username != "ww" && username != "xx")
        {
            cout << "该用户不存在" << endl;
            continue;
        }
        cout << "请输入密码" << endl;
        cin >> password;
        bool login = false;
        for (auto c : user_login)
        {
            if (username == c.username && password == c.password)
            {
                cout << "登录成功" << endl;
                login = true;
                break;
            }
        }
        if (login) break;
        else {
            cout << "登录失败" << endl;
            continue;
        }
    }
    return username;
}

DWORD WINAPI CreateSendMegThread(LPVOID lpParameter)
{
    SOCKET s_send = (SOCKET)lpParameter;
    cin.ignore();
    while (true)
    {
        char meg_send[maxSize] = {};
        int send_len = 0;
        cout << "请输入发送信息（向其他用户发信息需键入‘:’及用户名）:";
        //string line;
        cin.getline(meg_send, maxSize);
        //cout << "in:" << meg_send << endl;
        send_len = send(s_send, meg_send, maxSize, 0);
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
    WORD w_req = MAKEWORD(2, 2);//版本号2.2
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
    string userID;
    int send_len;
    userID = Login(user_login);                     //调用登录函数
    while (true)                                    //向服务器发送登录信息
    {
        char currentID[maxSize] = {};
        strcpy(currentID, userID.c_str());
        send_len = send(socketServer, currentID, maxSize, 0);
        if (send_len == SOCKET_ERROR) {
            cout << currentID << "用户登录服务器失败！"  << endl;
            continue;
        }
        else
        {
            cout << currentID << "用户登录服务器成功！" << endl;
            break;
        }
    }
    
    CreateThread(nullptr, 0, CreateRecvMegThread, (LPVOID)socketServer, 0, nullptr);
    CreateThread(nullptr, 0, CreateSendMegThread, (LPVOID)socketServer, 0, nullptr);
    while (true) {}

    closesocket(socketServer);
    WSACleanup();

    return 0;
}
