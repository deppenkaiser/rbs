cmake_minimum_required(VERSION 3.10)
project(rbs C)

add_executable(rbs
    rbs.c
    sm.c
)

target_include_directories(rbs PRIVATE .)