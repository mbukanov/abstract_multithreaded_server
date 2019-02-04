#ifndef BASESERVER_HPP
#define BASESERVER_HPP
#include <netinet/in.h>
#include "myThread.hpp"

#include <string>

class BaseServer: public myThread {
private:
    unsigned short port;
    int listen_fd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;

    class BaseServerException{};
    class SocketException : public BaseServerException {};
    class BindException   : public BaseServerException {};
    class ListenException : public BaseServerException {};
    class AcceptException : public BaseServerException {};

protected:
    void *thread_function();

public:
    BaseServer(const unsigned short port);
    ~BaseServer();
    int Receive(int socket_fd, unsigned char * buffer, const int size);
    int Receive(int socket_fd, std::string& buffer);
    void Send(int socket_fd, unsigned char * buffer, const int size);
    void Send(int socket_fd, std::string buffer);
    
    virtual void ConnectionHandler(int conn_fd) {};

};

#endif
