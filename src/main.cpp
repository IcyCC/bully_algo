#include <iostream>
#include "handy/handy.h"

int main() {
    auto loop = handy::EventLoop::GetInstance();

    auto task1 = [](){std::cout<<"test "<<handy::TimeMilli()<<std::endl;};
    auto  t1 = loop->CreateRepeatTask(task1, 1000);

    auto task2 = [t1](){
        t1->Cancel();
        std::cout<<"Cancel "<<std::endl;
    };
    auto t2 = loop->CreateDelayTask(task2, 5000);
    loop->RunLoop();

    return 0;
}