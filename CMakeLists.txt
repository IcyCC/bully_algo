cmake_minimum_required(VERSION 3.12)
project(bully_algo)

set(CMAKE_CXX_STANDARD 14)

add_executable(bully_algo src/main.cpp src/handy/event_loop.cpp  src/handy/event_loop.h src/handy/conn.cpp src/handy/conn.h src/handy/poller.cpp src/handy/poller.h src/handy/channel.cpp src/handy/channel.h src/handy/util.cpp src/handy/util.h src/node.cpp src/node.h)