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
    printf("BaseServer::BaseServer: port = %d\n", port);
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw SocketException();

    memset(&servaddr, 0, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int reuseaddr = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr));

    if(bind(listen_fd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
        throw BindException();

    if(listen(listen_fd, 1024) != 0)
        throw ListenException();
}

void* BaseServer::thread_function() {
    fd_set read_fds;
    fd_set write_fds;
    fd_set except_fds;

    for(;;) {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&except_fds);
    
        FD_SET(listen_fd, &read_fds);
        FD_SET(listen_fd, &except_fds);

        int i = 0;

        int high_socket = listen_fd;
        for(i = 0; i < peers.size(); i++) {
            FD_SET(peers[i]->socket, &read_fds);
//          FD_SET(peers[i]->socket, &write_fds);
            if(peers[i]->socket > high_socket)
                high_socket = peers[i]->socket;
        }


        int res = select(high_socket + 1, &read_fds, &write_fds, &except_fds, NULL);

        if(res < 0) {
            return NULL;
        } else if (res == 0 ) {
            return NULL;
        }
    
        if (FD_ISSET(listen_fd, &read_fds)) {
            CreateConnection();
        }

        for(i = (int)peers.size()-1; i >= 0; i--) {
            if(FD_ISSET(peers[i]->socket, &read_fds)) {
                if(ReceiveData(peers[i]) < 0) {
                    CloseConnection(&peers[i]);
                    peers.erase(peers.begin()+i);
                    continue;
                }
           }
           if(FD_ISSET(peers[i]->socket, &write_fds)) {
                if(SendData(peers[i]) < 0) {
                    CloseConnection(&peers[i]);
                    peers.erase(peers.begin()+i);
                    continue;
                }
            }
        }
    }

}


int BaseServer::ReceiveData(Peer *peer) {
    printf("BaseServer::ReceiveData\n");
    int received_count = recv(peer->socket, peer->data, 1024, 0);
    if(received_count <= 0) {
        return -1;
    } else {
        peer->received_count = received_count;
    }

    ReceiveCallback(peer);

    return 0;
}

int BaseServer::SendData(Peer *peer) {
    printf("BaseServer::SendData\n");
    int send_count = send(peer->socket, peer->data, peer->send_len, 0);
    printf("BaseServer::SendData: send_count = %d\n", send_count);
    peer->clearData();
    if(send_count <= 0) {
        return -1;
    } 
    return send_count;
}


BaseServer::~BaseServer() {
    printf("BaseServer::~BaseServer\n");
    Stop();
    close(listen_fd);
}

void BaseServer::CreateConnection() {
    printf("BaseServer::CreateConnection\n");
    struct sockaddr_in cliaddr;
    memset(&cliaddr, 0, sizeof(cliaddr));
    int connfd = 0;
    socklen_t len = sizeof(cliaddr);
    connfd = accept(listen_fd, (struct sockaddr*)&cliaddr, &len);
    if(connfd < 0)
        return;

    Peer * peer = new Peer(connfd, cliaddr);
    peers.push_back(peer);

    CreateConnectionCallback(peer);
}

char * Peer::getData() {
    return this->data;
}

void Peer::setData(const char *data, int len) {
    memcpy(this->data, data, len);
    this->send_len = len;
}

void Peer::clearData() {
    memset(data, 0, sizeof(data));
    send_len = 0;
}

std::string Peer::getAddress() {
    char peer_ipv4_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &this->address.sin_addr, peer_ipv4_str, INET_ADDRSTRLEN);
    char buffer[64] = "";
    sprintf(buffer, "%s:%d", peer_ipv4_str, this->address.sin_port );
    return std::string(buffer);
}


Peer::Peer(int connfd, struct sockaddr_in addr) {
    this->socket = connfd;
    this->address = addr;
    this->current_receiving_byte = 0;
}


void BaseServer::CloseConnection(Peer **peer) {
    printf("BaseServer::CloseConnection\n");
    if(*peer == NULL) return;
    close((*peer)->socket);
    delete *peer;
    *peer = NULL;
}
