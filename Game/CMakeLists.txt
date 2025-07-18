project(GameProjectTitle)

set(GUI_TYPE WIN32)

#Setup the source fiels of the Game Project
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
    ${CMAKE_CURRENT_BINARY_DIR}/Resources
    COMMENT "Copying resources into the binary folder"
)

add_dependencies(${PROJECT_NAME} copyResources)

#Filter the Source files
source_group("src" REGULAR_EXPRESSION ".cpp|.h")

#Define MATHEMATICAL_COORDINATESYSTEM to position the origin at the bottom-left corner of the screen
#When not defined, the origin is positioned at the top-left corner of the screen
target_compile_definitions(JelA_Engine PRIVATE MATHEMATICAL_COORDINATESYSTEM)

#Link Visual Leak Detector for MSVC
if (MSVC)
    find_path(VLD_INCLUDE_DIR vld.h PATHS "C:/Program Files (x86)/Visual Leak Detector/include")
    find_library(VLD_LIBRARY vld PATHS "C:/Program Files (x86)/Visual Leak Detector/lib/Win64")

    if (VLD_INCLUDE_DIR AND VLD_LIBRARY)
        target_include_directories(${PROJECT_NAME} PRIVATE ${VLD_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${VLD_LIBRARY})
        target_compile_definitions(${PROJECT_NAME} PRIVATE _CRTDBG_MAP_ALLOC)
        message(STATUS "VLD found and linked")
    else()
        message(WARNING "VLD not found. Memory leak detection will be disabled.")
    endif()
endif()