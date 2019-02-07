#ifndef BASESERVER_HPP
#define BASESERVER_HPP
#include <netinet/in.h>
#include "myThread.hpp"

#include <string>
#include <vector>

class BaseServer;

class Peer {
    friend class BaseServer;
private:
    int socket;
    struct sockaddr_in address;
    char data[1024];
    int received_count;
    int current_receiving_byte;

    char send_buffer[1024];
    int send_len;
public:
    Peer(int connfd, struct sockaddr_in addr);
    std::string getAddress();
    char * getData();
    void setData(const char *data, int len);
    void clearData();
};

class BaseServer: public myThread {
private:
    unsigned short port;
    int listen_fd;
    struct sockaddr_in servaddr;
    std::vector<Peer*> peers;

public:
    class BaseServerException{};
    class SocketException : public BaseServerException {};
    class BindException   : public BaseServerException {};
    class ListenException : public BaseServerException {};
    class AcceptException : public BaseServerException {};

    static int getPeersCount();

    void *thread_function();
    BaseServer(const unsigned short port);
    ~BaseServer();

    virtual void ReceiveCallback(Peer *peer) {}
    virtual void CreateConnectionCallback(Peer *peer) {}

    int ReceiveData(Peer *peer);
    int SendData(Peer *peer);

    void CreateConnection();
    void CloseConnection(Peer ** peer);



};

#endif
