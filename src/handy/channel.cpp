//
// Created by 苏畅 on 2019/6/2.
//

#include "channel.h"
#include <sys/poll.h>
#include<unistd.h>
#include <sys/socket.h>

namespace handy {
    Channel::Channel(handy::EventLoop *_base, int _fd, int _events) {
        base = _base;
        fd = _fd;
        events = _events;
    }

    Channel::~Channel(){
        Close();
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