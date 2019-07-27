cmake_minimum_required(VERSION 2.8)

project(AsyncLog)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wconversion -Wsizeof-pointer-memaccess \
                     -Wfloat-equal -Wconversion-null -Woverflow -Wshadow \
                     -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -pthread -g -O3")

include_directories(src test)

add_executable(Example
        test/Example.cpp
        src/SyncQueue.cpp
        src/SyncQueue.h
        src/BufferBlock.cpp
        src/BufferBlock.h)