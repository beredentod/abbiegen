cmake_minimum_required(VERSION 3.6)
project(abbiegen)

set(CMAKE_CXX_STANDARD 11)

add_executable(abbiegen files/main.cpp files/dijkstra.cpp files/graph.cpp)
target_link_libraries(abbiegen "-std=c++1z")

install(TARGETS abbiegen DESTINATION bin)

