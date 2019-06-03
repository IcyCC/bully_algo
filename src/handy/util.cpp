//
// Created by 苏畅 on 2019/6/2.
//

#include "util.h"
#include <chrono>
#include <memory>


namespace handy {
    int64_t TimeMicro() {
        std::chrono::time_point<std::chrono::system_clock> p = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(p.time_since_epoch()).count();
    };
}