//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_NODE_H
#define BULLY_ALGO_NODE_H

#include "handy/conn.h"
#include <string>
#include <map>
#include <functional>
#include <memory>

namespace bully {
    class Message {
    public:
        int  from;
        int to;
        std::string msg;
        Message(const std::string &  s);
        Message(int _from, int _to, std::string _msg){
            from = _from;
            to = _to;
            msg = _msg;
        };

        std::string ToString();

    };

    class Node {
    public:
        int id;
        std::map<int, std::pair<std::string, int>> neighbors;
        int leader_id;
        void PushNeighbor(int _id, const std::string & _host, int _port){
            auto p = std::make_pair(_host, _port);
            neighbors.insert(std::make_pair(_id, p));
            auto c = new handy::TcpConn(handy::EventLoop::GetInstance(), handy::BufferType::BUFF_CRLF);
            c->Connect(_host, _port, TIMEOUT, "0.0.0.0");
            c->OnRead(this->ComOnRead);
            neighbor_conns.insert(std::make_pair(_id, c));
        };
    private:
        int ELECTION_FREQ = 1000;
        int TIMEOUT = 1000;
    private:
        handy::EventLoop * loop;
    public:
        enum  NodeStateType{
            IDLE,
            ELECTING, // 选举中
            FLLOW, //从节点
            LEADER, // 主节点
        };
        NodeStateType nodeState;

        std::shared_ptr<handy::Timer> election_timeout_timer;
        std::shared_ptr<handy::Timer> ping_timer;
        std::shared_ptr<handy::Timer> ping_timeout_timer;
    private:
        handy::TcpServer *server;
        std::map<int, handy::TcpConn *> neighbor_conns;
    public:
        std::function<void (handy::TcpConn * conn)> ComOnRead;
        void Serve(); // 运行服务
        void election();
        void pingLeader();
        Node(int _id, int _port, int _leader_id, NodeStateType _state);
    };
}


#endif //BULLY_ALGO_NODE_H
