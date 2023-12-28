cmake_minimum_required(VERSION 3.26)
project(lab3m1 C)

set(CMAKE_C_STANDARD 11)

include_directories(ex1)
include_directories(ex2)
include_directories(milestone1)

add_executable(lab3m1
        ex1/dplist.c
        ex1/dplist.h
        ex1/program.c
        ex2/dplist.h
        milestone1/dplist.c
        milestone1/dplist.h
        milestone1/dplist_test.c
        milestone1/ourtest.c
        milestone1/randomtest.c)
