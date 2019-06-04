//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_EVENT_LOOP_H
#define BULLY_ALGO_EVENT_LOOP_H

#include <functional>
#include <queue>
#include <memory>
#include <vector>
#include <set>
#include "poller.h"
#include"util.h"

namespace handy {
    typedef std::function<void()> Task;

    inline  void DeafultTask (){

    };

    class EventLoop;

    class Timer : public noncopyable {
    public:
        int id;
        EventLoop *base;
        Task t;
        int64_t at; //s 执行时间
        bool done;
        bool running;
        int64_t repeat;

    public:
        Timer(const Task &task, int64_t _at, EventLoop *_base, int64_t _repeat);

        void setDone();

        bool Cancel();

    };

    class TimerPtrCompare {
    public:
        bool operator()(const std::shared_ptr<Timer> &t1, const std::shared_ptr<Timer> &t2) const {
            return t1->at > t2->at;
        }
    };

    class EventLoop {
    public:
        int timer_id = 0;
        std::set<int> cancel_timer_ids;
        std::priority_queue<std::shared_ptr<Timer>, std::vector<std::shared_ptr<Timer>>, TimerPtrCompare> timers;

        int getTimerId() {
            timer_id = timer_id + 1;
            return timer_id;
        };

        void CancelTimer(int timer_id);

    public:
        PollerBase *poller;
        static EventLoop *_self;

    private:
        EventLoop();

        ~EventLoop();

    public:

        static EventLoop *GetInstance();

        void loopOnce();

        void RunLoop();

        std::shared_ptr<Timer> CreateAtTimeTask(const Task &task, int64_t _at);

        std::shared_ptr<Timer> CreateDelayTask(const Task &task, int64_t time);

        std::shared_ptr<Timer> CreateRepeatTask(const Task &task, int64_t time);

    };


    class Channel : public noncopyable {
    public:
        // base为事件管理器，fd为通道内部的fd，events为通道关心的事件

        Channel(EventLoop *_base, int _fd, int _events);

        ~Channel();

        EventLoop *getBase() { return base; }

        //通道id
        //关闭通道
        void Close();

        //挂接事件处理器
        void OnRead(const Task &_readcb) { readcb = _readcb; }

        void OnWrite(const Task &_writecb) { writecb = _writecb; }

        void OnError(const Task &_errorcb) { errorcb = _errorcb; }

        void OnRead(Task &&_readcb) { readcb = std::move(_readcb); }

        void OnError(Task &&_errorcb) { errorcb = std::move(_errorcb); }

        void OnWrite(Task &&_writecb) { writecb = std::move(_writecb); }

        //启用读写监听
        void EnableRead(bool enable);

        void EnableWrite(bool enable);

        //处理读写事件
        void handleRead() { readcb(); }

        void handleWrite() { writecb(); }

        void handleError() { errorcb(); };

    public:
        EventLoop *base;
        int fd;
        short events;
        int64_t id;
        Task readcb, writecb, errorcb;
    };

}


#endif //BULLY_ALGO_EVENT_LOOP_H
