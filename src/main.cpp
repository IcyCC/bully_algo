#include <iostream>
#include "handy/handy.h"

int main() {
    auto loop = handy::EventLoop::GetInstance();

    auto task = [](){std::cout<<"test "<<handy::TimeMilli()<<std::endl;};
    loop->CreateReaptTask(task, 1000);

    loop->RunLoop();

    return 0;
}