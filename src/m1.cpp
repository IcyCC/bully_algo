#include <iostream>
#include "handy/handy.h"
#include "node.h"

int main() {
    auto loop = handy::EventLoop::GetInstance();

    auto n1 = bully::Node(1, 5001, 3, bully::Node::FLLOW);


        n1.PushNeighbor(2, "0.0.0.0", 5002);
        n1.PushNeighbor(3, "0.0.0.0", 5003);
    loop->RunLoop();

    return 0;
}