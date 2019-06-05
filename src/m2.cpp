#include <iostream>
#include "handy/handy.h"
#include "node.h"

int main() {
    auto loop = handy::EventLoop::GetInstance();

    auto n2 = bully::Node(2, 5002, 3, bully::Node::FLLOW);

        n2.PushNeighbor(1, "0.0.0.0", 5001);
        n2.PushNeighbor(3, "0.0.0.0", 5003);

    loop->RunLoop();

    return 0;
}