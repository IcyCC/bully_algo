//
// Created by 苏畅 on 2019/6/4.
//

#include <iostream>
#include "../src/handy/handy.h"

int main() {
    auto loop = handy::EventLoop::GetInstance();

    handy::TcpServer server("127.0.0.1", 5000);

    server.onConnCreate([](handy::TcpConn * c){
       std::cout<<"test"<<std::endl;
    });

    server.Bind();
    loop->RunLoop();
    return 0;
}