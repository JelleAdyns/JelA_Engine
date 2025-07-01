project(JelA_Engine)

file(GLOB SRC
     "*.h"
     "*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE YES)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "xinput"
    "shlwapi"
    "Dwrite"
    "d2d1"
    "mf"
    "mfplat"
    "mfuuid"
)

target_include_directories(${PROJECT_NAME}
    PUBLIC "."
)