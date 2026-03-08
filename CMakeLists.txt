cmake_minimum_required(VERSION 3.20)
project(Project_Name)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/W4 /WX)

add_definitions(-DUNICODE -D_UNICODE)

add_subdirectory(Engine)
add_subdirectory(Game)

