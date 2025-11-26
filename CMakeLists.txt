cmake_minimum_required(VERSION 4.0)
project(Logger)

set(CMAKE_CXX_STANDARD 23)

add_library(Logger STATIC Logger.cpp
)

add_executable(main main.cpp)

target_link_libraries(main Logger)