#include "MyServer.hpp"

MyServer::MyServer(const unsigned short port) 
    : BaseServer(port)
{}

void MyServer::MyServer(int connfd) {
    std::string recv_buffer;
    int res = Receive(connfd, recv_buffer);
    printf("MyServer::MyServer: receive message: %s\n", recv_buffer.c_str());

    printf("MyServer::MyServer: send message: %s\n", recv_buffer.c_str());
    Send(connfd, recv_buffer);
}
