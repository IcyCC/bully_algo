//
// Created by 苏畅 on 2019/6/5.
//

#include <iostream>
#include "handy/handy.h"
#include "node.h"

int main() {
    auto loop = handy::EventLoop::GetInstance();

    auto n3 = bully::Node(3, 5003, 3, bully::Node::LEADER);

        n3.PushNeighbor(1, "0.0.0.0", 5001);
        n3.PushNeighbor(2, "0.0.0.0", 5002);
    loop->RunLoop();

    return 0;
}