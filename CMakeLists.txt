cmake_minimum_required(VERSION 4.5.1)
project(Project_Name)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/W4 /WX)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
add_subdirectory(Engine)
add_subdirectory(Game)
add_subdirectory(Unit_Tests)