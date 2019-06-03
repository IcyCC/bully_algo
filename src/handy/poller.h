//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_POLLER_H
#define BULLY_ALGO_POLLER_H

#include"util.h"
#include <map>

namespace handy {
    class Channel;

    class PollerBase : private noncopyable {
    public:
        void AddChannel(Channel *ch);

        void RemoveChannel(Channel *ch);

        void UpdateChannel(Channel *ch);

        void loopOnce(int waitMs);

    private:
        std::map<int, handy::Channel *> _channels;
    };

} // namespace handy



#endif //BULLY_ALGO_POLLER_H
