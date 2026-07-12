function(AddVLD)

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

endfunction()