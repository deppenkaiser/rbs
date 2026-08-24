cmake_minimum_required(VERSION 3.8)
project(rbs C)

add_executable(rbs
	main.c
)

target_include_directories(rbs PRIVATE .)
