//
// Created by 苏畅 on 2019/6/2.
//
#include <sys/poll.h>
#include <exception>
#include "poller.h"
#include "event_loop.h"

namespace handy {
    void PollerBase::loopOnce(int waitMs) {
        struct pollfd (*files) = new pollfd[_channels.size()];
        int poll_size = 0;
        for (auto &it : _channels) {
            files[poll_size].fd = it.first;
            files[poll_size].events = it.second->events;
            poll_size++;
        }
        int flag = poll(files, nfds_t(poll_size), waitMs);
        if (flag == 0) {
            return;
        } else if (flag == -1) {
            throw std::runtime_error("Poller Fails");
        } else {

            // 触发到事件
            for (int i = 0; i < poll_size; i++) {
                if (files[i].revents | POLLIN) {
                    // 可读
                    _channels[files[i].fd]->handleRead();
                }
                if (files[i].revents | POLLOUT) {
                    // 可写
                    _channels[files[i].fd]->handleWrite();
                }
            }
        }
        delete[] files;
    }

    void PollerBase::AddChannel(Channel *ch) {
        _channels[ch->fd] = ch;
    }


    void PollerBase::RemoveChannel(Channel *ch) {
        _channels.erase(ch->fd);
    }

    void PollerBase::UpdateChannel(handy::Channel *ch) {
        _channels[ch->fd] = ch;
    }

}