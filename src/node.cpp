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
    loop = handy::EventLoop::GetInstance();
    id = _id;
    int max = id;
    for (auto _neighbor: _neighbors) {
        Neighbor neighbor;
        neighbor.id = _neighbor;
        if (max < _neighbor) {
            max = _neighbor;
        }
        neighbors.push_back(neighbor);
    }
    if (id == max) {
        state = state_leader;
    } else {
        state = state_follow;
    }
    server = handy::TcpServer("127.0.0.1", id);
    server.Bind();
    wait_success = false;
}

void Node::election() {
    for (auto neighbor: this->neighbors) {
        neighbor.neighbor_connection.Send("election " + std::to_string(neighbor.id));
    }
    this->state = state_election;
}


void Node::success() {
    for (auto neighbor: this->neighbors) {
        neighbor.neighbor_connection.Send("success " + std::to_string(neighbor.id));
    }
    this->state = state_leader;
    this->wait_success = false;
}


int largest_id(int id, std::vector<Neighbor> &neighbors) {
    int max = id;
    for (auto neighbor:neighbors) {
        if (max < neighbor.id) {
            max = neighbor.id;
        }
    }
    return max;
}

int Node::get_leader() {
    for (int i = 0; i < neighbors.size(); i++) {
        if (neighbors[i] == this->leader) {
            return i;
        }
    }
    this.id;
}

void Node::Serve() {

    leader = largest_id(id, neighbors);

    std::shared_ptr<handy::Timer> beat_task;

    for (auto neighbor:neighbors) {
        handy::TcpConn client(loop, "127.0.0.1", neighbor, 100, "127.0.0.1");
        neighbor.neighbor_connection = client;
    }

    auto end_beat_func = [this]() {
        this->beat_timer.Cancel();
        if (this->id != largest_id(this->id, this->neighbors)) {
            this->election();
        } else {
            this->success();
        }
        this->wait_success = false;
    };

    auto beat_func = [this]() {
        this->neighbors[this->get_leader()].neighbor_connection.Send("beat");
    };
    auto election_func = [this]() {
        if (this->id != largest_id(this->id, this->neighbors)) {
            this->election();
        } else {
            this->success();
        }
        this->wait_success = false;
    };

    auto success_func = [this]() {
        this->success();
    };

    if (this->state != state_leader) {
        this->election_timer = loop->CreateDelayTask(election_func, 5000);
        this->beat_timer = loop->CreateRepeatTask(beat_func, 5000);
        this->end_beat_timer = loop->CreateDelayTask(end_beat_func, 10000);
    }

    for (auto neighbor:neighbors) {
        neighbor.neighbor_connection.OnRead([this, &election_func, &success_func](handy::TcpConn *c) {
            auto data = c->Read();
            Message message(data);
            if (this->state == state_follow && message.op == "beat") {
                c->Send("alive");
            } else if (this->state == state_follow && message.op == "election" && atoi(message.params[0]) < this->id) {
                c->Send("exist " + std::to_string(this->id));
                this->election();
                this->success_timer = loop->CreateDelayTask(success_func, 5000);
            } else if (this->state == state_follow && message.op == "election" && atoi(message.params[0]) > this->id) {
                if (this->wait_success) {
                    this->wait_success_timer.Cancel();
                }
                this->wait_success_timer = loop->CreateDelayTask(election_func, 5000);
            } else if (this->state == state_follow && message.op == "exist") {
                if (this->wait_success) {
                    this->wait_success_timer.Cancel();
                }
                this->wait_success_timer = loop->CreateDelayTask(election_func, 5000);
            } else if (this->state == state_election && message.op == "election" &&
                       atoi(message.params[0]) > this->id) {
                this->state = state_follow;
                this->success_timer.Cancel();
                if (this->wait_success) {
                    this->wait_success_timer.Cancel();
                }
                this->wait_success_timer = loop->CreateDelayTask(election_func, 5000);
            } else if (this->state == state_election && message.op == "election" &&
                       atoi(message.params[0]) < this->id) {
                c->Send("exist " + std::to_string(this->id));
                this->success_timer = loop->CreateDelayTask(success_func, 5000);
            } else if (this->state == state_election && message.op == "exist") {
                this->state = state_follow;
                this->success_timer.Cancel();
                if (this->wait_success) {
                    this->wait_success_timer.Cancel();
                }
                this->wait_success_timer = loop->CreateDelayTask(election_func, 5000);
            } else if (message.op == "success") {
                this->state = state_follow;
                this->wait_success = false;
                this->leader = atoi(message.params[0])
                if (this->wait_success) {
                    this->wait_success_timer.Cancel();
                }
                this->wait_success_timer.Cancel();
                this->beat_timer = loop->CreateRepeatTask(beat_func, 5000);
                this->end_beat_timer = loop->CreateDelayTask(end_beat_func, 10000);
            } else if (message.op == "election" && this->id == largest_id(this->id, this->neighbors)) {
                this->success();
            } else if (message.op == "alive" && this->state == state_follow) {
                this->end_beat_timer.Cancel();
                this->end_beat_timer = loop->CreateDelayTask(end_beat_func, 5000)
            }
        })
    }

    auto beat_task = loop->CreateRepeatTask(beat_func, 1000);

    loop->RunLoop();
}
