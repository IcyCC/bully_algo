cmake_minimum_required(VERSION 3.12)
project(bully_algo)

set(CMAKE_CXX_STANDARD 14)

add_executable(bully_algo
        src/handy/event_loop.cpp
        src/handy/event_loop.h
        src/handy/conn.cpp
        src/handy/conn.h
        src/handy/poller.cpp
        src/handy/poller.h
        src/handy/util.cpp
        src/handy/util.h
        src/node.cpp
        src/node.h
        src/handy/handy.h
#        src/main.cpp
#        tests/timer_test.cpp
#        tests/conn_server_tests.cpp
##        tests/bully_node.cpp
       src/m1.cpp
#      src/m2.cpp
#        src/m3.cpp
        )
