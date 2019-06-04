//
// Created by 苏畅 on 2019/6/2.
//

#include "event_loop.h"
#include "poller.h"
#include <sys/poll.h>
#include<unistd.h>
#include <sys/socket.h>

namespace handy {

    void Timer::setDone() {
        done = true;
    }

    bool Timer::Cancel(){
        if (done){
            return false;
        }
        base->CancelTimer(id);
        return true;
    }

    Timer::Timer(const Task &_task, int64_t _at, EventLoop *_base, int64_t _repeat) {
        t = _task;
        at = _at;
        base = _base;
        repeat = _repeat;
        id = base->getTimerId();
        running = true;
    }

    EventLoop * EventLoop::_self = NULL;

    EventLoop::EventLoop() {
        poller = new PollerBase();
        timer_id = 0;
    }

    EventLoop::~EventLoop() {
        delete poller;
    }

    EventLoop  *EventLoop::GetInstance() {
        if (EventLoop::_self == NULL) {
            EventLoop::_self = new EventLoop();
        }
        return EventLoop::_self;
    }

    std::shared_ptr<Timer> EventLoop::CreateAtTimeTask(const Task &task, int64_t _at) {
        auto t = std::make_shared<Timer>(task, _at, this, 0);
        timers.push(t);
        return t;
    }

    std::shared_ptr<Timer> EventLoop::CreateDelayTask(const Task &task, int64_t time) {
        int64_t at = TimeMilli() + time;
        auto t = CreateAtTimeTask(task, at);
        return t;
    }

    std::shared_ptr<Timer> EventLoop::CreateRepeatTask(const Task &task, int64_t time) {
        int64_t at = TimeMilli() + time;
        auto t = std::make_shared<Timer>(task, at, this, time);
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
            if(t->at > now_time) {
                // 最近的任务小于当前时间
                break;
            } else {
                timers.pop();
                t->running = false;
                if (cancel_timer_ids.find(t->id) != cancel_timer_ids.end()) {
                    continue;
                }
                t->t();

                if (t->repeat > 0){
                    t->at = now_time + t->repeat;
                    t->running = true;
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

    void EventLoop::CancelTimer(int timer_id){
        cancel_timer_ids.insert(timer_id);
    }



    Channel::Channel(handy::EventLoop *_base, int _fd, int _events) {
        readcb = DeafultTask;
        writecb = DeafultTask;
        errorcb = DeafultTask;
        base = _base;
        fd = _fd;
        events = _events;
    }

    Channel::~Channel(){
        Close();
        base->poller->RemoveChannel(this);
    };

    void Channel::Close() {
        close(fd);
    }

    void Channel::EnableRead(bool enable){
        if (enable){
            events = events |  POLLIN;
        } else {
            events = events & (~POLLIN);
        }
        base->poller->UpdateChannel(this);
    };

    void Channel::EnableWrite(bool enable) {
        if (enable){
            events = events |  POLLOUT;
        } else {
            events = events & (~POLLOUT);
        }
        base->poller->UpdateChannel(this);
    }

}
