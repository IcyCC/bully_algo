//
// Created by 苏畅 on 2019/6/2.
//

#include "conn.h"
#include <sys/socket.h>
#include <fcntl.h>

namespace handy {
    void TcpConn::Send(const Buffer &msg)
    {
        send_buffer += msg;
    }

    void TcpConn::handleRead(TcpConn * con)
    {
        while (_state == State::Connected) {
            int rd;
            char buff[4096];
            if(_channel->fd >= 0) {
                memset(buff, 0, 4096);
                rd = read(_channel->fd, buff, 4096);
            }
            if(rd == -1 && errno == EINTR) {
                continue;
            } else if(rd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                if (readcb_ && read_buffer.size()) {
                    readcb_(con);
                }
                break;
            } else if(_channel->fd == -1 || rd == 0 || rd == -1) {
                // TODO: handle peer socket closed
                break;
            } else {
                read_buffer += buff;
                readcb_(con);
            }
        }
        
    }

    void TcpConn::handleWrite(TcpConn * con)
    {
        if(_state == State::Connected) {
            int len = send_buffer.size();
            int pos = 0;
            while(len) {
                std::string buff = send_buffer.substr(pos, 4096);
                auto sended = write(_channel->fd, buff.c_str(), buff.size());
                if (sended > 0) {
                    pos += sended;
                    len -= sended;
                } else if (sended == -1 && errno == EINTR) {
                    continue;
                } else if (sended == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {

                    break;
                }
            }
            if (!len && writablecb_) {
                writablecb_(con);
                send_buffer.clear();
            }
        }
    }

    TcpConn::TcpConn(EventLoop *base, int fd)
    {
        readcb_ = defaultTcpCallBack;
        writablecb_ = defaultTcpCallBack;
        statecb_ = defaultTcpCallBack;

        _base = base;
        _state = State::Connected;
        _channel = new Channel(base, fd, 0);
        _channel->EnableRead(true);
        _channel->EnableWrite(true);
        _channel->OnRead([this] { this->handleRead(this); });
        _channel->OnWrite([this] { this->handleWrite(this); });
        _base->poller->AddChannel(_channel);
    }

    TcpConn::TcpConn(EventLoop *base, const std::string &host, unsigned short port, int timeout, const std::string &localip)
    {
        readcb_ = defaultTcpCallBack;
        writablecb_ = defaultTcpCallBack;
        statecb_ = defaultTcpCallBack;

        auto peer_addr = IPv4Addr(host, port);
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        int r = 0;
        if(localip.size()) {
            auto local_addr = IPv4Addr(localip, 0);
            r = bind(fd, (struct sockaddr *) &local_addr.addr_, sizeof(struct sockaddr));
        }
        if(!r) {
            r = connect(fd, (struct sockaddr *) &peer_addr.addr_, sizeof(struct sockaddr));
        }
        if(r == 0) {
            // connect error
            exit(1);
        }
        _state = State::Connected;
        _base = base;
        _channel = new Channel(base, fd, 0);
        _channel->EnableRead(true);
        _channel->EnableWrite(true);
        _channel->OnRead([this] { this->handleRead(this); });
        _channel->OnWrite([this] { this->handleWrite(this); });
        _base->poller->AddChannel(_channel);
    }

    int TcpServer::Bind(bool reusePort)
    {
        auto addr = IPv4Addr(host, port);
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        int flag = 1;
        if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof flag)) {
            exit(1);
        }
    #ifdef SO_REUSEPORT
        flag = reusePort;
        if(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof flag)) {
            exit(1);
        }
    #endif
        if(bind(fd, (struct sockaddr *) &addr.addr_, sizeof(struct sockaddr))) {
            close(fd);
            exit(1);
        }
        if(listen(fd, 20)) {
            close(fd);
            exit(1);
        }
        delete _listen_channel;
        _listen_channel = new Channel(_base, fd, 0);
        _listen_channel->OnRead([this] { handleAccept(); });
        _base->poller->AddChannel(_listen_channel);
        _listen_channel->EnableRead(true);
        return 1;
    }

    void TcpServer::handleAccept()
    {
        struct sockaddr_in raddr;
        socklen_t rsz = sizeof(raddr);
        int lfd = _listen_channel->fd;
        int cfd;
        while(lfd >= 0 && (cfd = accept(lfd, (struct sockaddr *) &raddr, &rsz)) >= 0) {
            sockaddr_in peer, local;
            socklen_t alen = sizeof(peer);
            conns_map[cfd] = std::make_shared<TcpConn>(_base, cfd);
            createcb_(conns_map[cfd].get());
            statecb_(conns_map[cfd].get());
        }
    }
}
