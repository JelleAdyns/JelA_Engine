project(GameProjectTitle)

set(GUI_TYPE WIN32)

#Setup the source files of the Game Project
set(SOURCES 
	"Main.cpp"
	"Game.cpp"
	"Game.h"	
 )

#Create the Game Project
#You need to remove ${CMAKE_CURRENT_SOURCE_DIR}/Icons.rc if you don't have an Icon for the application
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/Icons.rc")

#Specify that the project is a WIN32 application (DON'T MODIFY THIS)
set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES)

#Link the JelA_Engine to the Game Project (DON'T MODIFY THIS)
add_dependencies(${PROJECT_NAME} JelA_Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE JelA_Engine)

#Set the Game Project as startup project
set_property(GLOBAL PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

#Copy the 'Resources' folder to the Binary Directory upon build
message(STATUS "BINARY DIR: ${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(copyResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
    COMMENT "Copying resources into the binary folder"
)

add_dependencies(${PROJECT_NAME} copyResources)

#Filter the Source files
source_group("src" REGULAR_EXPRESSION ".cpp|.h")


include(functions)
AddVLD()