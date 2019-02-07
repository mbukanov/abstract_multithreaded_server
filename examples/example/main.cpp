#include "MyServer.hpp"

int main() {

    MyServer server(5551);
    server.Start();
    server.Wait();


    return 0;
}
