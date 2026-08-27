project(Tests)

#set(GUI_TYPE WIN32)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Fetching Google Tests")
    cmake_policy(SET CMP0135 NEW)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    message(STATUS "Fetching Google Tests Succesfull!")

endif()

set(SOURCES
        "Main.cpp"
        FixedSizeAllocatorsTests.cpp
        SingleLinkAllocatorsTests.cpp
        ObservingObjects.cpp
)

add_executable(${PROJECT_NAME}  ${SOURCES})

#set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES)

add_dependencies(${PROJECT_NAME} JelA_Engine)
target_link_libraries(${PROJECT_NAME} PRIVATE JelA_Engine gtest)


include(functions)
AddVLD()

enable_testing()
include(GoogleTest)
gtest_discover_tests(Tests)