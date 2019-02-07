#ifndef MYSERVER_HPP
#define MYSERVER_HPP
#include "BaseServer.hpp"

class MyServer : public BaseServer 
{
public:
    MyServer(const unsigned short port);
    void ReceiveCallback(Peer *peer);
};

#endif
