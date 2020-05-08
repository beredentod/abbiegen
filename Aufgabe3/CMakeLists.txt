cmake_minimum_required(VERSION 3.6)
project(abbiegen-raw)

set(CMAKE_CXX_STANDARD 11)

add_executable(abbiegen-raw files/main.cpp files/dijkstra.cpp files/graph.cpp)
target_link_libraries(abbiegen-raw "-std=c++1z")

install(TARGETS abbiegen-raw DESTINATION bin)

