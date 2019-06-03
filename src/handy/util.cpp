//
// Created by 苏畅 on 2019/6/2.
//

#include "util.h"
#include <chrono>
#include <memory>
#include <cstring>
#include <arpa/inet.h>


namespace handy {
    int64_t TimeMicro() {
        std::chrono::time_point<std::chrono::system_clock> p = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(p.time_since_epoch()).count();
    };

    IPv4Addr::IPv4Addr(const std::string &host, unsigned short port) {
        memset(&addr_, 0, sizeof addr_);
        addr_.sin_family = AF_INET;
        addr_.sin_port = htons(port);
        if(host.empty()) {
            addr_.sin_addr.s_addr = INADDR_ANY;
        } else {
            if(!inet_aton(host.c_str(), &(addr_.sin_addr))) {
                exit(1);
            }
        }
    }
}