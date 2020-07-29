// CMake_chat.cpp: 定义应用程序的入口点。
//

#include "CMake_chat.h"
#include<iostream>
#include<stack>
#include<vector>
#include<cstdio>
#include<string>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;
#define maxSize 1024
vector<SOCKET> sockets;

typedef struct userInformition
{
    string username;
    SOCKET user_socket;
}node_user;
//定义向量用于存储用户ID信息
vector<node_user> user_sockets;


DWORD WINAPI CreateClientThread(LPVOID lpParameter)
{
    SOCKET sock_clt = (SOCKET)lpParameter;
    //sockets.push_back(sock_clt);
    char meg_recv[maxSize] = {};
    //定义长度变量
    int send_len = 0;
    int recv_len = 0;
    cout << "创建线程成功" << endl;
    
    int userID;
    char username[maxSize] = {'\0'};                        //建立客户端用户ID数组
    while (true)
    {
        recv_len = recv(sock_clt, username, maxSize, 0);    //接受用户名
        cout << username << "用户已登录" << endl;
        if (recv_len != SOCKET_ERROR)
            break;
    }

    bool user_isExist;
    if (!user_sockets.empty())                              //更新客户端用户信息
    {
        user_isExist = false;
        for (auto c : user_sockets)
        {
            if (c.username == username)
            {
                user_isExist = true;
                c.user_socket = sock_clt;
                break;
            }
        }
    }
    else user_isExist = false;
    if (!user_isExist)
    {
        node_user currentUser;
        currentUser.username = username;
        currentUser.user_socket = sock_clt;
        user_sockets.push_back(currentUser);
    }

    while (true)
    {
        recv_len = recv(sock_clt, meg_recv, maxSize, 0);
        cout << "接收消息：" << meg_recv << endl;
        string s_toUser = meg_recv;
        if (user_sockets.size() > 1)
        {
            //cout << user_sockets[0].username << user_sockets[1].username << endl;
            for (auto c : user_sockets)         //根据报文":"后指向的用户名查找要发送的用户的SOCKET
            {
                if (s_toUser.find(":" + c.username) == 0)      
                {
                    s_toUser.erase(0, c.username.size()+1);
                    strcpy(meg_recv, s_toUser.c_str());
                    send_len = send(c.user_socket, meg_recv, sizeof(meg_recv), 0);
                    if (send_len < 0)
                        cout << "发送失败" << endl;
                    else 
                        cout << "发送消息至客户端:" << c.user_socket << endl;
                    break;
                }
            }
            
        }
        else cout << "当前只有一个客户端" << endl;
        memset(meg_recv, 0, sizeof(meg_recv));      //清空数据缓冲区
    }
    closesocket(sock_clt);
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

    //定义长度变量
    int send_len = 0;
    int recv_len = 0;
    int len = 0;
    //定义发送缓冲区和接受缓冲区
    char send_buf[maxSize];
    char recv_buf[maxSize];
    //定义服务端套接字，接受请求套接字
    SOCKET socketServer;
    SOCKET socketAccept;
    //服务端地址客户端地址
    SOCKADDR_IN server_addr;                 //一种通用的套接字地址。
    SOCKADDR_IN accept_addr;

    //填充服务端信息
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//ADDR_ANY
    server_addr.sin_port = htons(1234);                  //传递端口 htons：将主机字节顺序转换为网络字节顺序
    //创建套接字
    socketServer = socket(AF_INET, SOCK_STREAM, 0);         //ADDRESS FAMILY协议族，流式套接字类型，
    if (bind(socketServer, (SOCKADDR*)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
        cout << "套接字绑定失败！" << endl;
        WSACleanup();
    }
    else {
        cout << "套接字绑定成功！" << endl;
    }
    //设置套接字为监听状态
    if (listen(socketServer, SOMAXCONN) < 0) {
        cout << "设置监听状态失败！" << endl;
        WSACleanup();
    }
    else {
        cout << "设置监听状态成功！" << endl;
    }
    cout << "服务端正在监听连接，请稍候...." << endl;
    //接受连接请求
    /*len = sizeof(SOCKADDR);
    socketAccept = accept(socketServer, (SOCKADDR*)&accept_addr, &len);
    if (socketAccept == SOCKET_ERROR) {
        cout << "连接失败！" << endl;
        WSACleanup();
        return 0;
    }*/
    
    //接收数据
    while (true)
    {
        len = sizeof(SOCKADDR);
        socketAccept = accept(socketServer, (SOCKADDR*)&accept_addr, &len);
        if (socketAccept == SOCKET_ERROR) {
            cout << "连接失败！" << endl;
            WSACleanup();
            return 0;
        }
        else cout << "连接建立，准备接受数据" << endl;
        auto sock_thread = ::CreateThread(nullptr, 0, CreateClientThread, (LPVOID)socketAccept, 0, nullptr);
        if (sock_thread == NULL) {
            cerr << "创建线程失败! Error code: " << ::WSAGetLastError() << endl;
            WSACleanup();
            //system("pause");
            return 0;
        }
        ::CloseHandle(sock_thread);
    }
    //关闭套接字
    closesocket(socketServer);
    closesocket(socketAccept);
    //释放DLL资源
    WSACleanup();
    return 0;
}
