cmake_minimum_required(VERSION 3.22)
project(lab7 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab7 main.c
        uthreads/uthread.c
        uthreads/uthread.h)
