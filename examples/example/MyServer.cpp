#include "MyServer.hpp"

MyServer::MyServer(const unsigned short port) : BaseServer(port) {}

void MyServer::ReceiveCallback(Peer *peer) {
    printf("peer data = %s\n", peer->getData());
    char buffer[124]= "";
    sprintf(buffer, "Echo Receive: %s\n", peer->getData());
    peer->setData(buffer, 124);
    SendData(peer);
}
