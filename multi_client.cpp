#include <unistd.h>
#include <cstring>

#include <functional>
#include <iostream>
#include <string>
#include <fstream>

#include "src/Buffer.h"
#include "src/Socket.h"
#include "src/Connection.h"
#include "src/ThreadPool.h"
#include "src/util.h"
#include "src/EventLoop.h"

static int id = 0;


void OneClient(int msgs) {
  Socket *sock = new Socket();
  InetAddress *addr = new InetAddress("127.0.0.1", 8888);
  sock->Connect(addr);
  Connection *conn = new Connection(new EventLoop(nullptr), sock->GetFd());
  // sleep(1);
  int count = 0;
  while (count < msgs) {
    conn->SetWriteBuf("I'm client!");
    conn->Write();
    if (conn->GetState() == Connection::State::Closed) {
      conn->Close();
      break;
    }
    conn->Read();
    std::cout << "msg count " << count++ << ": " << conn->GetReadBuf()->GetStr() << std::endl;
  }
  delete conn;
  delete addr;
}


int main(int argc, char *argv[]) {
    int size = 8;
    ThreadPool *pool = new ThreadPool(size);
    std::function<void()> func = std::bind(OneClient, 10);
    for (int i = 0; i < 100000; ++i) {
        pool->Add(func);
    }
    delete pool;
    std::cout << "Finish" << std::endl;

    return 0;
}
