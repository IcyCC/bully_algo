//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_POLLER_H
#define BULLY_ALGO_POLLER_H
#include"util.h"
#include"channel.h"

namespace handy
{
    class PollerBase : private noncopyable {
        public:
        int64_t id_;
        int lastActive_;

        void AddChannel(Channel *ch) ;
        void RemoveChannel(Channel *ch) ;
        void UpdateChannel(Channel *ch);
        void loopOnce(int waitMs);
};

} // namespace handy



#endif //BULLY_ALGO_POLLER_H
