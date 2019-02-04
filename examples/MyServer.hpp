#include "BaseServer.hpp"

class MyServer : public BaseServer {
public:
    MyServer(const unsigned short port);
    void ConnectionHandler(int connfd);
};

