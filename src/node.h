//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_NODE_H
#define BULLY_ALGO_NODE_H

#include <vector>
#include "./handy/handy.h"
#include <string>

#define state_beat 0
#define state_leader 1
#define state_election 2
#define state_follow 3

class Message {
public:
    std::string op;
    std::vector<std::string> params;
public:
    Message(std::string &raw);
};


struct Neighbor {
    int id;
    handy::TcpConn neighbor_connection;
    int election;
};

class Node {
private:
    handy::EventLoop *loop;
    handy::TcpServer &server;
public:
    int id;
    int state;
    int leader;

    bool wait_success;
    std::vector<Neighbor> &neighbors;

    std::shared_ptr<handy::Timer> success_timer;
    std::shared_ptr<handy::Timer> wait_success_timer;
    std::shared_ptr<handy::Timer> election_timer;
    std::shared_ptr<handy::Timer> beat_timer;
    std::shared_ptr<handy::Timer> end_beat_timer;


public:
    Node(int _id, std::vector<int> &_neighbors);

    int get_leader();

    void election();

    void success();

    void Serve(); // 运行服务
};


#endif //BULLY_ALGO_NODE_H
