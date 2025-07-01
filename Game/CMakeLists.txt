project(GameProjectTitle)

set(GUI_TYPE WIN32)

#Setup the source fiels of the Game Project
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


#Create the Game Project
#You need to remove ${CMAKE_CURRENT_SOURCE_DIR}/Icons.rc if you don't have an Icon for the application
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/Icons.rc")

#Specify that the project is a WIN32 application
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES)

#Link the JelA_Engine to the Game Project
add_dependencies(${PROJECT_NAME} JelA_Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE JelA_Engine)

#Set the Game Project as startup project
set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

#Copy the 'Resources' folder to the 'out' folder upon build
message(STATUS "BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(copyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    ${CMAKE_CURRENT_BINARY_DIR}/Resources
    COMMENT "Copying resources into the binary folder"
)

add_dependencies(${PROJECT_NAME} copyResources)


#Define MATHEMATICAL_COORDINATESYSTEM to position the origion at the bottom-left corner of the screen
#When not defined, the origin is positioned at the top-left corner of the screen
target_compile_definitions(JelA_Engine PRIVATE MATHEMATICAL_COORDINATESYSTEM)