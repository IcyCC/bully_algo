//
// Created by 苏畅 on 2019/6/2.
//

#include "event_loop.h"

namespace handy {

    void Timer::setDone() {
        done = true;
    }

    Timer::Timer(handy::Task *_task, int64_t _at, EventLoop *_base, int64_t _repeat) {
        t = _task;
        id = base->timer_id;
        at = _at;
        base = _base;
        repeat = _repeat;
    }

    EventLoop::EventLoop() {
        poller = new PollerBase();
        timer_id = 0;
    }

    EventLoop::~EventLoop() {
        delete poller;
    }

    EventLoop *EventLoop::GetInstance() {
        if (self == NULL) {
            EventLoop::self = new EventLoop();
        }
        return self;
    }

    Timer *EventLoop::CreateAtTimeTask(handy::Task &task, int64_t _at) {
        timer_id ++;
        auto t = new Timer(&task, _at, this, -1);
        timers.push(t);
        return t;
    }

    Timer *EventLoop::CreateDelayTask(Task &task, int64_t time) {
        timer_id ++;
        int64_t at = TimeMilli() + time;
        auto t = CreateAtTimeTask(task, at);
        return t;
    }

    Timer * EventLoop::CreateReaptTask(Task &task, int64_t time) {
        timer_id ++;
        int64_t at = TimeMilli() + time;
        auto t = new Timer(&task, at, this, time);
        timers.push(t);
        return t;
    }

    void EventLoop::loopOnce() {
        // io loop
        poller->loopOnce(0);

        // timer loop
        while (true) {
            if (timers.empty()) {
                break;
            }
            auto t = timers.top();
            auto now_time = TimeMilli();
            if(t->at < now_time) {
                // 最近的任务小于当前时间
                break;
            } else {
                timers.pop();
                t->t();

                if (t->repeat > 0){
                    t->at = now_time + t->repeat;
                    timers.push(t);
                } else {
                    t->setDone(); // 执行
                }
            }
        }
    }

    void EventLoop::RunLoop() {
        while(true) {
            loopOnce();
        }
    }

}
