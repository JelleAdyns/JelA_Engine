project(GameProjectTitle)

set(GUI_TYPE WIN32)

set(SOURCES 
	"Main.cpp"
	"Game.cpp"
	"Game.h"	
)

foreach(SOURCE_FILE IN LISTS SOURCES)
        list(APPEND SOURCES "src/${SOURCE_FILE}")
endforeach()

list(FILTER SOURCES INCLUDE REGEX "src/")

foreach(SOURCE_FILE IN LISTS SOURCES)
        message(STATUS "Source File Path: ${SOURCE_FILE}")
endforeach()

add_executable(${PROJECT_NAME} WIN32 ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/Icons.rc")
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES)

add_dependencies(${PROJECT_NAME} JelA_Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE JelA_Engine)

set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

message(STATUS "BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(copyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_CURRENT_BINARY_DIR}/Resources
    COMMENT "Copying resources into the binary folder"
)

add_dependencies(${PROJECT_NAME} copyResources)