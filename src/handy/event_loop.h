//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_EVENT_LOOP_H
#define BULLY_ALGO_EVENT_LOOP_H

#include <poll.h>
#include <functional>
#include <queue>
#include <vector>
#include"util.h"
#include "poller.h"


namespace handy {
    typedef std::function<void()> Task;

    class EventLoop;

    class Timer : public noncopyable {
    public:
        int id;
        EventLoop *base;
        Task *t;
        int64_t at; //s 执行时间
        bool done;
        int64_t repeat;
    public:
        Timer(Task *task, int64_t _at, EventLoop *_base, int64_t _repeat);


        void setDone();

        static bool cmp(const Timer &lhs, const Timer &rhs) {
            return lhs.at < rhs.at;
        }
    };

    class EventLoop {
    public:
        int timer_id;
        std::priority_queue<Timer *, std::vector<Timer *>, decltype(Timer::cmp)> timers;
    public:
        PollerBase *poller;
    private:
        EventLoop();

        ~EventLoop();

        static EventLoop *self;
    public:

        EventLoop *GetInstance();

        void loopOnce();

        void RunLoop();

        Timer *CreateAtTimeTask(Task &task, int64_t _at);

        Timer *CreateDelayTask(Task &task, int64_t time);

        Timer *CreateReaptTask(Task &task, int64_t time);

    };
}


#endif //BULLY_ALGO_EVENT_LOOP_H
