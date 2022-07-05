#include "src/EventLoop.h"
#include "src/Server.h"
#include "src/Connection.h"
#include "src/Socket.h"
#include "src/Log.h"
#include "src/Buffer.h"
#include "src/Timer.h"
#include "src/HttpConn.h"

#include <iostream>
#include <string>


int main() {
    Server *server = new Server("127.0.0.1", 8888);
    
    server->SetProcessConnCallback([](Connection *conn) {
        conn->Read();

        LOG_INFO("Message from client " + std::to_string(conn->GetFd()) + ": " + conn->GetReadBuf()->GetStr());
        std::cout << "Message from client " << conn->GetFd() << ": " << conn->GetReadBuf()->GetStr() << std::endl;

        HttpRequest request;
        int ret = request.Parse(conn->GetReadBuf()->GetStr());
        
        
        conn->SetWriteBuf(conn->GetReadBuf()->GetStr().c_str());
        conn->Write();
    });


    server->Launch();
    delete server;

    return 0;
}


