cmake_minimum_required(VERSION 3.8)
project(ipc_example)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(ipc_example ${SOURCE_FILES})