//
// Created by 苏畅 on 2019/6/2.
//

#include "conn.h"

namespace handy {
    void TcpConn::Send(Buffer &msg)
    {
        send_buffer += msg;
    }

    void TcpConn::handleRead(const TcpConn &con)
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
            } else if(rd = -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                if (readcb_ && read_buffer.size()) {
                    readcb_(con);
                }
                break;
            } else if(_channel->fd == -1 || rd == 0 || rd == -1) {
                // TODO: handle peer socket closed
                break;
            } else {
                read_buffer += buff;
            }
        }
        
    }

    void TcpConn::handleWrite(const TcpConn &con)
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

    int TcpServer::Bind(const std::string &host, unsigned short port, bool reusePort = false)
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
    }
}
