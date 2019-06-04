//
// Created by 苏畅 on 2019/6/4.
//

#include <iostream>
#include "../src/handy/handy.h"

int main() {
    auto loop = handy::EventLoop::GetInstance();

    handy::TcpServer server("127.0.0.1", 5000);

    server.OnConnCreate([](handy::TcpConn * c){
       std::cout<<"服务器收到链接"<<std::endl;
    });

    server.OnConnRead([](handy::TcpConn * c){
        std::cout<<c->ReadBuffer()<<std::endl;
        c->Send("OK!");
    });
    server.Bind();

    handy::TcpConn client(loop);
    client.Connect("127.0.0.1", 5000, 100, "127.0.0.1");

    client.OnConnected([](handy::TcpConn * c){
        std::cout<<"连接到服务器"<<std::endl;
    });

    loop->CreateDelayTask([&client](){
        client.Send("HI");
    }, 100);

    client.OnRead([](handy::TcpConn * c){
        std::cout<<"收到服务器消息"<<c->ReadBuffer()<<std::endl;
    });
    loop->RunLoop();
    return 0;
}