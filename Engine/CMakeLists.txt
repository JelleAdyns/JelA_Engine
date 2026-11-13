project(JelA_Engine)

set(LIB_HEADERS
        include/JelA/Commands/Command.h
        include/JelA/Commands/MovementCommands.h
        include/JelA/Components/Renderers/LineRenderer.h
        include/JelA/Components/Renderers/PrimitiveRenderers.h
        include/JelA/Components/Renderers/Renderer.h
        include/JelA/Components/Renderers/TextRenderer.h
        include/JelA/Components/Renderers/TextureRenderer.h
        include/JelA/Components/Renderers/ShapeRenderers.h
        include/JelA/Components/Movement.h
        include/JelA/Components/Rotator.h
        include/JelA/Components/Component.h
        include/JelA/Components/RectTransform.h
        include/JelA/Components/Sprite.h
        include/JelA/Components/TextureImage.h
        include/JelA/Components/Transform.h
        include/JelA/Managers/InputManager.h
        include/JelA/Managers/RenderManager.h
        include/JelA/Managers/ResourceManager.h
        include/JelA/Managers/SceneManager.h
        include/JelA/Memory/FixedSizeAllocators.h
        include/JelA/Memory/MemoryAllocator.h
        include/JelA/Memory/SingleLinkAllocators.h
        include/JelA/Misc/Defines.h
        include/JelA/Misc/FileExceptions.h
        include/JelA/Misc/framework.h
        include/JelA/Misc/HResultHandler.h
        include/JelA/Misc/Concepts.h
        include/JelA/AudioService.h
        include/JelA/Controller.h
        include/JelA/DirectXObjects.h
        include/JelA/Engine.h
        include/JelA/GameObject.h
        include/JelA/GameWindow.h
        include/JelA/Geometry.h
        include/JelA/Observer.h
        include/JelA/ObservingObjects.h
        include/JelA/Scene.h
        include/JelA/Structs.h
        include/JelA/Utils.h
)
set(LIB_SOURCES
        ${LIB_HEADERS}
        src/AudioService.cpp
        src/Controller.cpp
        src/DirectXObjects.cpp
        src/Engine.cpp
        src/Memory/FixedSizeAllocators.cpp
        src/GameObject.cpp
        src/GameWindow.cpp
        src/Geometry.cpp
        src/Misc/HResultHandler.cpp
        src/Managers/InputManager.cpp
        src/ObservingObjects.cpp
        src/Components/RectTransform.cpp
        src/Managers/RenderManager.cpp
        src/Managers/ResourceManager.cpp
        src/Components/Rotator.cpp
        src/Scene.cpp
        src/Managers/SceneManager.cpp
        src/Memory/SingleLinkAllocators.cpp
        src/Components/Sprite.cpp
        src/Components/Renderers/TextRenderer.cpp
        src/Components/TextureImage.cpp
        src/Components/Renderers/TextureRenderer.cpp
        src/Components/Transform.cpp
        src/Utils.cpp
        src/XAudioService.cpp
        src/Components/Renderers/LineRenderer.cpp
        src/Components/Renderers/PrimitiveRenderers.cpp
)


add_library(${PROJECT_NAME} STATIC ${LIB_SOURCES})

if(USE_MATHEMATICAL_COORDINATESYSTEM)
    target_compile_definitions(JelA_Engine PUBLIC MATHEMATICAL_COORDINATESYSTEM)
endif()
target_sources(${PROJECT_NAME} PUBLIC FILE_SET HEADERS BASE_DIRS include FILES ${LIB_HEADERS})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
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

