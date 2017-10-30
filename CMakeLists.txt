cmake_minimum_required(VERSION 3.8)
project(ipc_example)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)
add_executable(ipc_example ${SOURCE_FILES})

set(UD_SOCK_SERVER_SOURCE_FILES ud-sock-server.c)
add_executable(unix_socket_server ${UD_SOCK_SERVER_SOURCE_FILES})