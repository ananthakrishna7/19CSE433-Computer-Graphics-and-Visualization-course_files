cmake_minimum_required(VERSION 3.10)
project(Assignment1 VERSION 1.0.0)

include_directories(include)

add_executable(assignment2 src/main.c src/glad.c)

target_link_libraries(assignment2 glfw)
