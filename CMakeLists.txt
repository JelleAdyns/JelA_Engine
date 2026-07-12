cmake_minimum_required(VERSION 3.20)
project(Project_Name)

#Define MATHEMATICAL_COORDINATESYSTEM to position the origin at the bottom-left corner of the screen
#When not defined, the origin is positioned at the top-left corner of the screen
option(USE_MATHEMATICAL_COORDINATESYSTEM "Origin positioned in bottom left" TRUE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/W4 /WX)

add_definitions(-DUNICODE -D_UNICODE)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
add_subdirectory(Engine)
add_subdirectory(Game)
add_subdirectory(Tests)

if (USE_MATHEMATICAL_COORDINATESYSTEM)
add_compile_definitions(MATHEMATICAL_COORDINATESYSTEM)
endif ()