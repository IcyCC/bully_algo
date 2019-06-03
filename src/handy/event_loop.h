//
// Created by 苏畅 on 2019/6/2.
//

#ifndef BULLY_ALGO_EVENT_LOOP_H
#define BULLY_ALGO_EVENT_LOOP_H

#include <poll.h>
#include <functional>
#include <queue>
#include"util.h"
#include "poller.h"


namespace handy {
    typedef  std::function<void()> Task;

    class Timer : public noncopyable {
        private:
            Task t; 
            int _at; //s 执行时间
        public: 
            bool Cancel();
    };

    class EventLoop {
        private:
            std::priority_queue<Timer*> _timers;
        public:
            PollerBase * poller;
        public:
            void loopOnce();
            void RunLoop();

            void CreateDelayTask(Task &task, int time);
            void CreateAtTimeTask(Task &task, int _at); 
            void CreateReaptTask(Task &task, int time);
            
    };
}


#endif //BULLY_ALGO_EVENT_LOOP_H
