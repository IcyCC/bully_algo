//
// Created by 苏畅 on 2019/6/2.
//

#include "node.h"
#include <cstdlib>

namespace bully {
    Message::Message(const std::string& s){
        std::vector<std::string> v = handy::SpliteString(s, " ");
        from = std::atoi(v[0].c_str());
        to = std::atoi(v[1].c_str());
        msg = v[2];
    };

    std::string Message::ToString(){
        return std::to_string(from) + " " + std::to_string(to) + " " + msg + "\r\n";
    };


    Node::Node(int _id, int _port, int _leader_id, bully::Node::NodeStateType _state) {

        this->ComOnRead = [this](handy::TcpConn * conn){
            auto msg = Message(conn->Read());

            if (this->nodeState == NodeStateType::ELECTING && msg.msg == "Answer"){
                // 当s前节点为选举中 并且收到了节点的响应, 取消选举
                this->election_timeout_timer->Cancel();
            }

            else if (this->nodeState == NodeStateType::FLLOW && msg.msg == "Election") {
                // 当前节点为选举中 并且收到了选举请求 做出回应
                conn->Send("Answer");
            }

            else if (this->nodeState == NodeStateType::LEADER && msg.msg == "Ping") {
                // 当前节点为leader 收到了ping回复pong
                conn->Send("Pong");
            } else if (this->nodeState == NodeStateType::FLLOW && msg.msg == "Pong"){
                // 当前节点为从节点 收到了pong响应 取消心跳超时的选举
                this->ping_timeout_timer->Cancel();
            }
        };

        id = _id;
        leader_id = _leader_id;
        nodeState = _state;
        server = new handy::TcpServer("0.0.0.0", _port, handy::BufferType::BUFF_CRLF);
        server->OnConnRead(ComOnRead);
        server->Bind();
    }
    void Node::Serve() {
        this->ping_timer = loop->CreateRepeatTask([this](){
            if (nodeState == NodeStateType::FLLOW) {
                 // 从节点每秒发送一次心跳
                 this->pingLeader();
                 // 发送超时了开始选举
                this->ping_timeout_timer = loop->CreateDelayTask([this](){
                      this->nodeState = NodeStateType::ELECTING;

                    this->election(); //发送选举消息
                    // 选举超时了设置自己为leader
                    this->election_timeout_timer = loop->CreateDelayTask([this](){
                        this->nodeState = LEADER;
                    }, TIMEOUT);
                },TIMEOUT);
            }


            }, ELECTION_FREQ); // 建立心跳任务

        loop->RunLoop();
    }

}