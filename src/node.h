//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_NODE_H
#define BULLY_ALGO_NODE_H

#include <vector>
#include <string>


class Message {
public:
    std::string op;
    std::vector<std::string> params;
public:
    Message(std::string &raw);
};

class Node {
public:
    int id;
    int state;
    std::vector<int> neighbors;
public:
    Node(int _id, std::vector<int> &_neighbors);

    void election();

    void Serve(); // 运行服务
};


#endif //BULLY_ALGO_NODE_H
