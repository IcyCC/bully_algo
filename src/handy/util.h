    //
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_UTIL_H
#define BULLY_ALGO_UTIL_H

#include<string>

namespace handy {
    struct noncopyable {
    protected:
        noncopyable() = default;
        virtual ~noncopyable() = default;

        noncopyable(const noncopyable &) = delete;
        noncopyable &operator=(const noncopyable &) = delete;
    };

    typedef std::string Buffer;
}
#endif //BULLY_ALGO_UTIL_H
