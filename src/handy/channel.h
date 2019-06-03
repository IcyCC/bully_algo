//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_CHANNEL_H
#define BULLY_ALGO_CHANNEL_H
#include <functional>
#include "util.h"
#include "event_loop.h"
namespace handy {
    class Channel : public noncopyable{
    public:
        // base为事件管理器，fd为通道内部的fd，events为通道关心的事件
        Channel(handy::EventLoop *_base, int _fd, int _events);
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



#endif //BULLY_ALGO_CHANNEL_H
