#include "BaseServer.hpp"

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

BaseServer::BaseServer(const unsigned short port) {
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw SocketException();

    printf("BaseServer::BaseServer, port = %d\n", port);
    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int reuseaddr = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));

	printf("BaseServer::run()\n");
    if(bind(listen_fd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
        throw BindException();

    if(listen(listen_fd, 1024) != 0)
        throw ListenException();


}

void* BaseServer::thread_function() {
    printf("BaseServer::thread_function\n");
    socklen_t len = 0;
    int pid_t childpid = -1;
    int connfd = 0;
	for(;;) {
        len = sizeof(cliaddr);
        printf("Base_server::thread_function::accepting\n");
        connfd = accept(listen_fd, (sockaddr*)&cliaddr, &len);
        if(connfd == -1)
            throw AcceptException();
        if((childpid = fork()) == 0) {
            close(listen_fd);
            
            // Connection handler.
            ConnectionHandler(connfd);
            close(connfd);
            exit(1);
        }
    }
}

BaseServer::~BaseServer() {
    printf("BaseServer::~BaseServer\n");
    Stop();
    close(listen_fd);
}

int BaseServer::Receive(int socket_fd, unsigned char * buffer, const int size) {
    printf("BaseServer::Receive\n");
    return read(socket_fd, buffer, size);
}

int BaseServer::Receive(int socket_fd, std::string &str) {
    printf("BaseServer::Receive\n");
    const int size = 512;
    unsigned char buffer[size];
    int res = read(socket_fd, buffer, size);
    std::string tmp_str(buffer, buffer+size);
    str = tmp_str;
    return res;
}

void BaseServer::Send(int socket_fd, unsigned char * buffer, const int size) {
    printf("BaseServer::Send\n");
    write(socket_fd, buffer, size);
}

void BaseServer::Send(int socket_fd, std::string str) {
    printf("BaseServer::Send\n");
    const int size = str.length()+1;
    unsigned char buffer[size];
    std::copy(str.begin(), str.end(), buffer);
    buffer[size-1] = 0;
    write(socket_fd, buffer, size);
}






