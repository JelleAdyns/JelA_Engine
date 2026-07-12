project(JelA_Engine)

file(GLOB SRC
     "include/*.h"
     "src/*.cpp"
)

add_library(${PROJECT_NAME} STATIC ${SRC})

if(USE_MATHEMATICAL_COORDINATESYSTEM)
    target_compile_definitions(JelA_Engine PUBLIC MATHEMATICAL_COORDINATESYSTEM)
endif()

target_include_directories(${PROJECT_NAME}
        PUBLIC "./include"
        PRIVATE "./src"
)
target_link_libraries(${PROJECT_NAME} PRIVATE
        "xinput"
        "shlwapi"
        "Dwrite"
        "d2d1"
        "d3d11"
        "dxgi"
        "dxguid"
        "mf"
        "mfplat"
        "mfuuid"
        "xaudio2"
)

