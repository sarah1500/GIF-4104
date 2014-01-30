cmake_minimum_required(VERSION 2.6)
project (test2)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

find_package( Threads )
add_executable(test2 main.cpp)
target_link_libraries( test2 ${CMAKE_THREAD_LIBS_INIT} )