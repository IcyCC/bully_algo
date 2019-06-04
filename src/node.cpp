//
// Created by 苏畅 on 2019/6/2.
//

#include "node.h"
#include "util.h"
#include "./handy/handy.h"


Message::Message(std::string &raw) {
    std::vector<std::string> parts = SplitString(raw, " ");
    if (!parts.empty()) {
        op = parts[0];
        parts.erase(parts.begin());
        params = parts;
    }
}

Node::Node(int _id, std::vector<int> &_neighbors) {
    id = _id;
    neighbors = _neighbors;
}

int largest_id(int id, std::vector<int> neighbors) {
    int max = id;
    for (auto neighbor:neighbors) {
        if (max < neighbor) {
            max = neighbor;
        }
    }
    return max;
}

int get_index(int id, std::vector<int> neighbors) {
    for (int i = 0; i < neighbors.size(); i++) {
        if (neighbors[i] == id) {
            return i;
        }
    }
}

void Node::Serve() {

    int leader = largest_id(id, neighbors);

    auto loop = handy::EventLoop::GetInstance();

    handy::TcpServer server("127.0.0.1", id);

    std::vector<handy:: & TcpConn> neighbor_connections;

    for (auto neighbor:neighbors) {
        handy::TcpConn client(loop, "127.0.0.1", neighbor, 100, "127.0.0.1");
        neighbor_connections.push_back(client);
    }

    server.OnConnected([](handy::TcpConn *c) {
        auto resv_data = c->ReadBuffer();
        Message message(resv_data);
        if (message.op == "beat") {
            c->Send("alive");
        } else if (message.op == "election") {
            if (id == largest_id(id, neighbors)) {
                leader = id;
                for (auto neighbor_connection:neighbor_connections) {
                    neighbor_connection.Send("success " + std::to_string(id));
                }
            } else if (id > atoi(message.params[0])) {
                c->Send("exist " + std::to_string(id))
                for (auto neighbor_connection:neighbor_connections) {
                    neighbor_connection.Send("election " + std::to_string(id));
                }
            }
        } else if (message.op == "success") {
            leader = atoi(message.params[0]);
        }
    });
    server.Bind();

    auto beat_func = []() {
        if (id != leader) {
            neighbor_connections[get_index(id, neighbor_connections)].Send("beat");
        }
    };
    auto beat_task = loop->CreateRepeatTask(beat_func, 1000);

    loop->RunLoop();
}
