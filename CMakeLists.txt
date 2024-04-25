cmake_minimum_required(VERSION 3.10)

project(arfmipssim)

file(GLOB SRC "src/*.c")

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

add_executable(arfmipssim ${SRC})
