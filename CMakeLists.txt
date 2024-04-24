cmake_minimum_required(VERSION 3.10)

project(arfmipssim)

file(GLOB SRC "src/*.c")

add_executable(arfmipssim ${SRC})
