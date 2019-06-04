//
// Created by 苏畅 on 2019/6/2.
//

#include "node.h"
#include <cstdlib>

namespace bully {
    Message::Message(const std::string &s) {
        std::vector<std::string> v = handy::SpliteString(s, " ");
        from = std::atoi(v[0].c_str());
        to = std::atoi(v[1].c_str());
        msg = v[2];
    };

    std::string Message::ToString() {
        auto res = std::to_string(from) + " " + std::to_string(to) + " " + msg + " \r\n";
        return res;
    };


    Node::Node(int _id, int _port, int _leader_id, bully::Node::NodeStateType _state) {
        loop = handy::EventLoop::GetInstance();
        this->ComOnRead = [this](handy::TcpConn *conn,const std::string &res) {
            auto msg = Message(res);
            if (this->nodeState == NodeStateType::ELECTING && msg.msg == "Answer") {
                // 当s前节点为选举中 并且收到了节点的响应, 取消选举
                this->election_timeout_timer->Cancel();
                this->nodeState = NodeStateType ::FLLOW;
                handy::PutLog(std::to_string(id)+ " 收到 "+ std::to_string(msg.from) + " 响应"+", 选举取消");
            } else if (msg.msg == "Election") {
                // 当前节点为选举中 并且收到了选举请求 做出回应
                neighbor_conns[msg.from]->Send(Message(id, msg.from, "Answer").ToString());
            } else if (msg.msg == "Ping") {
                // 当前节点为leader 收到了ping回复pong
                neighbor_conns[msg.from]->Send(Message(id, msg.from, "Pong").ToString());
            } else if (this->nodeState == NodeStateType::FLLOW && msg.msg == "Pong") {
                // 当前节点为从节点 收到了pong响应 取消心跳超时的选举
                this->ping_timeout_timer->Cancel();
            } else if (msg.msg == "Victory") {
                handy::PutLog(std::to_string(id)+ " 收到 "+ " 响应"+", 更换leader "+std::to_string(msg.from));
                this->election_timeout_timer->Cancel();
                this->nodeState = NodeStateType::FLLOW;
                this->leader_id = msg.from;
            }
        };

        id = _id;
        leader_id = _leader_id;
        nodeState = _state;
        server = new handy::TcpServer("0.0.0.0", _port, handy::BufferType::BUFF_CRLF);
        server->OnConnMsg(ComOnRead);
        server->Bind();
        this->ping_timer = loop->CreateRepeatTask([this]() {
            if (nodeState == NodeStateType::FLLOW) {
                // 从节点每秒发送一次心跳
                this->pingLeader();
                handy::PutLog(std::to_string(id) + "检测心跳 " + "当前leader " + std::to_string(this->leader_id));

                // 发送超时了开始选举
                ping_timeout_timer = loop->CreateDelayTask([this]() {
                    handy::PutLog("检测心跳超时 开始选举 " + std::to_string(id));
                    this->nodeState = NodeStateType::ELECTING;
                    this->election(); //发送选举消息
                    // 选举超时了设置自己为leader
                    election_timeout_timer = loop->CreateDelayTask([this]() {
                        handy::PutLog("选举超时"  + std::to_string(id) +" 成为leader ");
                        victory();
                    }, TIMEOUT);
                }, TIMEOUT);
            }


        }, ELECTION_FREQ); // 建立心跳任务

    }

    void Node::Serve() {

        loop->RunLoop();
    }


    void Node::election() {
        bool is_max = true;
        for (auto &i : neighbor_conns) {
            if (i.first > id) {
                neighbor_conns[i.first]->Send(Message(id, i.first, "Election").ToString());
                is_max = false;
            }
        }

        if (is_max) {
            victory();
        }
    };

    void Node::pingLeader() {
        if (this->leader_id != id) {
            neighbor_conns[leader_id]->Send(Message(id, leader_id, "Ping").ToString());
        }
    }

    void Node::victory() {
        this->nodeState = LEADER;
        this->leader_id = id;
        for (auto &i : neighbor_conns) {
            neighbor_conns[i.first]->Send(Message(id, i.first, "Victory").ToString());
        }
    }

    void Node::stop() {
       loop->poller->RemoveChannel(this->server->_listen_channel);
       for (auto i : this->server->conns_map) {
           loop->poller->RemoveChannel(i.second->_channel);
       }
    }
}

