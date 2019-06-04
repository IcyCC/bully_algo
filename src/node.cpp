//
// Created by 苏畅 on 2019/6/2.
//

#include "node.h"

namespace bully {
    Message::Message(const std::string& s){

    };

    void Node::Serve() {
        ping_timer = loop->CreateRepeatTask([this](){
            if (nodeState == NodeStateType::FLLOW) {
                 // 从节点每秒发送一次心跳
                 this->pingLeader();
                ping_timout_timer = loop->CreateDelayTask([this](){
                      this->nodeState = NodeStateType::ELECTING;

                    this->election(); //发送选举消息

                    this->election_timeout_timer = loop->CreateDelayTask([this](){
                        this->nodeState = LEADER;
                    }, 1000);
                },
                1000);
            }


            }, 1000); // 建立心跳任务
        loop->RunLoop();
    }

    void Node::ComOnRead(handy::TcpConn * conn) {
        auto msg = Message(conn->Read());

        if (this->nodeState == NodeStateType::ELECTING && msg.msg == "Answer"){
            // 当前节点为选举中 并且收到了节点的响应, 取消选举
            election_timeout_timer->Cancel();
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
            this->ping_timout_timer->Cancel();
        }
    }

}