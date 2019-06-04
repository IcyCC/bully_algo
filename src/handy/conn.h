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
    typedef std::function<void(TcpConn * conn, const std::string& msg)> TcpDataCallBack;
    inline void defaultTcpCallBack(TcpConn * conn){

    };

    inline void defaultTcpDataBack(TcpConn * conn, const std::string & msg){

    };
    class TcpConn : public noncopyable {
        public:
        private:
            TcpCallBack readcb_, writablecb_, statecb_, errcb_, conncb_, disconncb_;
            TcpDataCallBack msgcb_;
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
            void OnMsg(const TcpDataCallBack &cb) { msgcb_ = cb; }
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
            void handleError(TcpConn* con) {
                errcb_(this); 
                cleanup(this);
            }
        private: 
            void cleanup(TcpConn * con);
    };

    class TcpServer: public noncopyable {
        public:
        TcpServer(const std::string &_host, unsigned short _port, BufferType type = BufferType::BUFF_CRLF) {
            host = _host;
            port = _port;
            _base = EventLoop::GetInstance();
            _listen_channel = NULL;

            readcb_ = defaultTcpCallBack;
            msgcb_ = defaultTcpDataBack;
            statecb_ = defaultTcpCallBack;
            createcb_= defaultTcpCallBack;
            _type = type;
        }
            std::string host;
            unsigned int port;
            EventLoop * _base;
    public:
            Channel *_listen_channel;
            std::map<int, std::shared_ptr<TcpConn>> conns_map;
            TcpCallBack statecb_, readcb_ ,createcb_ , errcb_, disconncb_;
            TcpDataCallBack msgcb_;
            BufferType _type;
            void handleAccept();
        public:

            int Bind(bool reusePort = false);
            void OnConnError(const TcpCallBack &cb) { errcb_ = cb; }
            void OnConnDisConnect(const TcpCallBack &cb) { disconncb_ = cb; };
            void OnConnCreate(const TcpCallBack &cb) { createcb_ = cb; }
            void OnConnState(const TcpCallBack &cb) { statecb_ = cb; }
            void OnConnRead(const TcpCallBack &cb) {
                readcb_ = cb;
                            }
            // 消息处理与Read回调冲突，只能调用一个
            void OnConnMsg(const TcpDataCallBack &cb) {
                msgcb_ = cb;
            }
    };
} // namespace handy

#endif //BULLY_ALGO_CONN_H
