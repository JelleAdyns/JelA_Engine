project(GameProjectTitle)

set(GUI_TYPE WIN32)

set(SOURCES 
	"main.cpp"
	"Game.cpp"
	"Game.h"	
)

add_executable(${PROJECT_NAME} WIN32  ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES)

add_dependencies(${PROJECT_NAME} EngineLibrary)
target_link_libraries(${PROJECT_NAME} PRIVATE EngineLibrary)

set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

message(STATUS "BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(copyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_CURRENT_BINARY_DIR}/Resources
    COMMENT "Copying resources into the binary folder"
)

add_dependencies(${PROJECT_NAME} copyResources)