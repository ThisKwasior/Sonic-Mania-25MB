project(RetroEngine)

add_executable(RetroEngine ${RETRO_FILES})

set(RETRO_SUBSYSTEM "DX9" CACHE STRING "The subsystem to use")
option(USE_PORTAUDIO "Whether or not to use PortAudio or default to XAudio." OFF)

set(DEP_PATH windows)

message(NOTICE "configuring for the " ${RETRO_SUBSYSTEM} " subsystem")

if(RETRO_SUBSYSTEM STREQUAL "DX9")
    target_link_libraries(RetroEngine
        d3d9
        # d3dcompiler
        xinput
        comctl32
        gdi32
        winmm
        ole32
        xaudio2_8
        ksuser
    )
elseif(RETRO_SUBSYSTEM STREQUAL "DX11")
    target_link_libraries(RetroEngine
        d3d11
        d3dcompiler
        DXGI
        XInput
    )
elseif(RETRO_SUBSYSTEM STREQUAL "OGL")
    find_package(glfw3 CONFIG)

    if(NOT glfw3_FOUND)
        message("could not find glfw, attempting to build from source")
        add_subdirectory(dependencies/ogl/glfw)
    else()
        message("found GLFW")
    endif()

    find_package(GLEW CONFIG)

    if(NOT GLEW_FOUND)
        message(NOTICE "could not find glew, attempting to build from source")

    else()
        message("found GLEW")
        add_library(glew ALIAS GLEW::GLEW)
    endif()

    target_link_libraries(RetroEngine
        glew
        glfw
    )
elseif(RETRO_SUBSYSTEM STREQUAL "VK")
    find_package(glfw3 CONFIG)

    if(NOT glfw3_FOUND)
        message("could not find glfw, attempting to build from source")
        add_subdirectory(dependencies/ogl/glfw)
    else()
        message("found GLFW")
    endif()

    find_package(Vulkan REQUIRED)

    target_compile_definitions(RetroEngine VULKAN_USE_GLFW=1)
    target_link_libraries(RetroEngine
        glfw
        Vulkan::Vulkan
    )
elseif(RETRO_SUBSYSTEM STREQUAL "SDL2")
    find_package(SDL2 CONFIG REQUIRED) # i ain't setting this up all the way
    target_link_libraries(RetroEngine 
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    )
else()
    message(FATAL_ERROR "RETRO_SUBSYSTEM must be one of DX9, DX11, OGL, VK, or SDL2")
endif()

if(USE_PORTAUDIO)
    if(RETRO_SUBSYSTEM STREQUAL "DX9" OR RETRO_SUBSYSTEM STREQUAL "DX11")
        message(FATAL_ERROR "portaudio not supported for DX9 and DX11.")
    endif()
    find_package(portaudio CONFIG)

    if(NOT portaudio_FOUND)
        message("could not find portaudio, attempting to build from source")

        if(EXISTS dependencies/all/portaudio)
            add_subdirectory(dependencies/all/portaudio)
        else()
            add_subdirectory(dependencies/${DEP_PATH}/portaudio)
        endif()
    else()
        message("found portaudio")
        target_link_libraries(RetroEngine 
            $<IF:$<TARGET_EXISTS:portaudio>,portaudio,portaudio_static>
        )
    endif()

    target_compile_definitions(RetroEngine PRIVATE RETRO_AUDIODEVICE_PORT=1)
endif()

target_compile_definitions(RetroEngine PRIVATE _CRT_SECURE_NO_WARNINGS)
target_link_libraries(RetroEngine
    winmm
    comctl32
)

if(RETRO_MOD_LOADER)
    set_target_properties(RetroEngine PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(RetroEngine PRIVATE -Wno-microsoft-cast -Wno-microsoft-exception-spec)
endif()
    
target_sources(RetroEngine PRIVATE ${RETRO_NAME}/${RETRO_NAME}.rc)
target_link_options(RetroEngine PRIVATE -mwindows)
