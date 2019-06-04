    //
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_UTIL_H
#define BULLY_ALGO_UTIL_H

#include<string>
#include <netinet/in.h>

namespace handy {
    struct noncopyable {
    protected:
        noncopyable() = default;
        virtual ~noncopyable() = default;

        noncopyable(const noncopyable &) = delete;
        noncopyable &operator=(const noncopyable &) = delete;
    };

    int64_t TimeMicro();
    inline int64_t TimeMilli() { return TimeMicro() / 1000; }

    typedef std::string Buffer;

    struct IPv4Addr {
        struct sockaddr_in addr_;
        IPv4Addr(const std::string &host, unsigned short port);
        IPv4Addr(const struct sockaddr_in &addr) : addr_(addr) {};
    };
}
#endif //BULLY_ALGO_UTIL_H
