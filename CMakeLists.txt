cmake_minimum_required(VERSION 3.8)

project(rbs LANGUAGES CXX)

add_executable(rbs)

target_sources(rbs PRIVATE
    main.cpp
)
