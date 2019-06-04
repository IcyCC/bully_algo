//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_CONN_H
#define BULLY_ALGO_CONN_H
#include<string>
#include<functional>
#include <sys/socket.h>
#include<unistd.h>
#include<cstring>
#include<map>
#include <errno.h>
#include"util.h"
#include"event_loop.h"

namespace handy
{

    class TcpConn;

    typedef std::function<void(TcpConn * conn)> TcpCallBack;
    inline void defaultTcpCallBack(TcpConn * conn){

    };
    class TcpConn : public noncopyable {
        public:
        private:
            TcpCallBack readcb_, writablecb_, statecb_, msgcb_, errcb_, conncb_, disconncb_;
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
            Buffer *send_buffer, *read_buffer;
        public:
            void Send(const std::string &msg);

            std::string Read() {
                auto b = read_buffer->GetBuffer();
                read_buffer->Clear();
                return b;
            };

            std::string ReadBuffer() {
                return read_buffer->GetBuffer();
            };
            void OnConnected(const TcpCallBack &cb) { conncb_ = cb; }
            void OnMsg(const TcpCallBack &cb) { msgcb_ = cb; }
            void OnError(const TcpCallBack &cb) { errcb_ = cb; }
            void OnDisConnted(const TcpCallBack &cb) { disconncb_ = cb; }
            void OnRead(const TcpCallBack &cb) {
                readcb_ = cb;
            };
            //当tcp缓冲区可写时回调
            void OnWritable(const TcpCallBack &cb) { writablecb_ = cb; }
            // tcp状态改变时回调
            void OnState(const TcpCallBack &cb) { statecb_ = cb; }

        public:
            TcpConn(EventLoop *base, BufferType type = BufferType::BUFF_CRLF);
            void Attach(int fd);
            void Connect(const std::string &host, unsigned short port, int timeout, const std::string &localip = "");
            ~TcpConn() { delete _channel; }
            void handleRead(TcpConn * con);
            void handleWrite(TcpConn* con);
            void cleanup(TcpConn * con);
        public: 
    };

    class TcpServer: public noncopyable {
        public:
        TcpServer(const std::string &_host, unsigned short _port, BufferType type = BufferType::BUFF_CRLF) {
            host = _host;
            port = _port;
            _base = EventLoop::GetInstance();
            _listen_channel = NULL;

            readcb_ = defaultTcpCallBack;
            msgcb_ = defaultTcpCallBack;
            statecb_ = defaultTcpCallBack;
            createcb_= defaultTcpCallBack;
            _type = type;
        }
            std::string host;
            unsigned int port;
            EventLoop * _base;
        private:
            Channel *_listen_channel;
            std::map<int, std::shared_ptr<TcpConn>> conns_map;
            TcpCallBack statecb_, readcb_, msgcb_ ,createcb_ , errcb_, disconncb_;
            BufferType _type;
            void handleAccept();
        public:

            int Bind(bool reusePort = false);
            void onConnError(const TcpCallBack &cb) { errcb_ = cb; }
            void onConnDisConnect(const TcpCallBack &cb) { disconncb_ = cb; };
            void onConnCreate(const TcpCallBack &cb) { createcb_ = cb; }
            void onConnState(const TcpCallBack &cb) { statecb_ = cb; }
            void onConnRead(const TcpCallBack &cb) {
                readcb_ = cb;
                            }
            // 消息处理与Read回调冲突，只能调用一个
            void onConnMsg(const TcpCallBack &cb) {
                msgcb_ = cb;
            }
    };
} // namespace handy

#endif //BULLY_ALGO_CONN_H
