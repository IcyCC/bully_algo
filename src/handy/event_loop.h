//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_EVENT_LOOP_H
#define BULLY_ALGO_EVENT_LOOP_H

#include <functional>
#include <queue>
#include <vector>
#include "poller.h"
#include"util.h"

namespace handy {
    typedef std::function<void()> Task;

    class EventLoop;

    class Timer : public noncopyable {
    public:
        int id;
        EventLoop *base;
        Task t;
        int64_t at; //s 执行时间
        bool done;
        int64_t repeat;
    public:
        Timer(const Task &task, int64_t _at, EventLoop *_base, int64_t _repeat);
        void setDone();

        bool operator <(const Timer& d)
        {
            return at < d.at;
        }
    };

    class EventLoop {
    public:
        int timer_id;
        std::priority_queue<Timer *> timers;
    public:
        PollerBase *poller;
        static EventLoop * _self;
    private:
        EventLoop();

        ~EventLoop();
    public:

        static EventLoop *GetInstance();

        void loopOnce();

        void RunLoop();

        Timer *CreateAtTimeTask(const Task &task, int64_t _at);

        Timer *CreateDelayTask(const Task &task, int64_t time);

        Timer *CreateReaptTask(const Task &task, int64_t time);

    };


    class Channel : public noncopyable{
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
        void OnRead(Task &&_readcb) { readcb = std::move(_readcb); }
        void OnWrite(Task &&_writecb) { writecb = std::move(_writecb); }

        //启用读写监听
        void EnableRead(bool enable);
        void EnableWrite(bool enable);

        //处理读写事件
        void handleRead() { readcb(); }
        void handleWrite() { writecb(); }

    public:
        EventLoop *base;
        int fd;
        short events;
        int64_t id;
        Task readcb, writecb, errorcb;
    };

}


#endif //BULLY_ALGO_EVENT_LOOP_H
