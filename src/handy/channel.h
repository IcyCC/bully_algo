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

        // base为事件管理器，fd为通道内部的fd，events为通道关心的事件
        Channel(EventLoop *base, int fd, int events);
        ~Channel();
        EventLoop *getBase() { return base_; }
        int fd() { return fd_; }
        //通道id
        int64_t id() { return id_; }
        short events() { return events_; }
        //关闭通道
        void close();

        //挂接事件处理器
        void onRead(const Task &readcb) { readcb_ = readcb; }
        void onWrite(const Task &writecb) { writecb_ = writecb; }
        void onRead(Task &&readcb) { readcb_ = std::move(readcb); }
        void onWrite(Task &&writecb) { writecb_ = std::move(writecb); }

        //启用读写监听
        void enableRead(bool enable);
        void enableWrite(bool enable);
        void enableReadWrite(bool readable, bool writable);
        bool readEnabled();
        bool writeEnabled();

        //处理读写事件
        void handleRead() { readcb_(); }
        void handleWrite() { writecb_(); }

    protected:
        EventLoop *base_;
        int fd_;
        short events_;
        int64_t id_;
        std::function<void()> readcb_, writecb_, errorcb_;
    };
}



#endif //BULLY_ALGO_CHANNEL_H
