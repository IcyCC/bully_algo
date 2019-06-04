//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_NODE_H
#define BULLY_ALGO_NODE_H

#include "handy/conn.h"
#include <string>
#include <map>
#include <memory>

namespace bully {
    class Message {
    public:
        int  from;
        int to;
        std::string msg;
        Message(const std::string &  s);

    };
    class Node {
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
        std::shared_ptr<handy::Timer> ping_timout_timer;

    public:
        void ComOnRead(handy::TcpConn * conn);
        void Serve(); // 运行服务
        void election(){};
        void pingLeader(){};
    };
}


#endif //BULLY_ALGO_NODE_H
