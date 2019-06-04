#include <iostream>
#include "handy/handy.h"
#include "node.h"

int main() {
    auto loop = handy::EventLoop::GetInstance();

    auto n1 = bully::Node(1, 5001, 3, bully::Node::FLLOW);


    auto n2 = bully::Node(2, 5002, 3, bully::Node::FLLOW);

    auto n3 = bully::Node(3, 5003, 3, bully::Node::LEADER);


    n1.PushNeighbor(2, "0.0.0.0", 5002);
    n1.PushNeighbor(3, "0.0.0.0", 5003);


    n2.PushNeighbor(1, "0.0.0.0", 5001);
    n2.PushNeighbor(3, "0.0.0.0", 5003);


    n3.PushNeighbor(1, "0.0.0.0", 5001);
    n3.PushNeighbor(2, "0.0.0.0", 5002);

    loop->CreateDelayTask([&n3](){
        n3.stop();
        }, 6000);

    loop->RunLoop();

    return 0;
}