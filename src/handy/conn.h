//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_CONN_H
#define BULLY_ALGO_CONN_H
#include<string>
#include<functional>
#include<unistd.h>
#include<cstring>
#include"util.h"
#include"event_loop.h"

namespace handy
{
    class TcpConn : public noncopyable {
        public:
            typedef std::function<void(const TcpConn &)> TcpCallBack;
        private:
            TcpCallBack readcb_, writablecb_, statecb_;
        public:
          enum State {
            Invalid = 1,
            Handshaking,
            Connected,
            Closed,
            Failed,
            } _state;
        private:
            EventLoop *_base;
            Channel *_channel;
            Buffer send_buffer, read_buffer;
        public:
            void Send(Buffer &msg);

            void OnRead(const TcpCallBack &cb) {
                readcb_ = cb;
            };
            //当tcp缓冲区可写时回调
            void OnWritable(const TcpCallBack &cb) { writablecb_ = cb; }
            // tcp状态改变时回调
            void OnState(const TcpCallBack &cb) { statecb_ = cb; }

        public:
            void handleRead(const TcpConn &con);
            void handleWrite(const TcpConn &con);
        public: 
    };

    class TcpServer: public noncopyable {
        private:
            TcpConn::TcpCallBack statecb_, readcb_, msgcb_ ,createcb_;
        public:
            int Bind(const std::string &host, unsigned short port, bool reusePort = false);
            void onConnCreate(const TcpConn::TcpCallBack &cb) { createcb_ = cb; }
            void onConnState(const TcpConn::TcpCallBack &cb) { statecb_ = cb; }
            void onConnRead(const TcpConn::TcpCallBack &cb) {
                readcb_ = cb;
                            }
            // 消息处理与Read回调冲突，只能调用一个
            void onConnMsg(const TcpConn::TcpCallBack &cb) {
                msgcb_ = cb;
            }
    };
} // namespace handy

#endif //BULLY_ALGO_CONN_H
