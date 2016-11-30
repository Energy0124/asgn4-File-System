cmake_minimum_required(VERSION 3.6)
project(asgn4_File_System)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    asg4-func.c
    asg4-vfs.c)

add_executable(asgn4_File_System ${SOURCE_FILES})