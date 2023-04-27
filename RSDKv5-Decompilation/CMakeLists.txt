cmake_minimum_required(VERSION 3.7)

project(RetroEngine)

set(DECOMP_VERSION 1.1.0-a)

set(RETRO_REVISION 3 CACHE STRING "What revision to compile for. Defaults to v5U = 3")
option(RETRO_MOD_LOADER "Enables or disables the mod loader." ON)
option(RETRO_DISABLE_PLUS "Disable plus. should be set on for any public releases." OFF)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

set(RETRO_NAME "RSDKv5")

set(RETRO_EMBEDDED_DATA False)

if(RETRO_REVISION STREQUAL "3")
    set(RETRO_NAME "RSDKv5U")
endif()

set(RETRO_OUTPUT_NAME ${RETRO_NAME} CACHE STRING "The exported name")

set(RETRO_FILES
    RSDKv5/main.cpp
    RSDKv5/RSDK/Core/RetroEngine.cpp
    RSDKv5/RSDK/Core/Math.cpp
    RSDKv5/RSDK/Core/Reader.cpp
    RSDKv5/RSDK/Core/Link.cpp
    RSDKv5/RSDK/Core/ModAPI.cpp
    RSDKv5/RSDK/Dev/Debug.cpp
    RSDKv5/RSDK/Storage/Storage.cpp
    RSDKv5/RSDK/Storage/Text.cpp
    RSDKv5/RSDK/Graphics/Drawing.cpp
    RSDKv5/RSDK/Graphics/Scene3D.cpp
    RSDKv5/RSDK/Graphics/Animation.cpp
    RSDKv5/RSDK/Graphics/Sprite.cpp
    RSDKv5/RSDK/Graphics/Palette.cpp
    RSDKv5/RSDK/Graphics/Video.cpp
    RSDKv5/RSDK/Audio/Audio.cpp
    RSDKv5/RSDK/Input/Input.cpp
    RSDKv5/RSDK/Scene/Scene.cpp
    RSDKv5/RSDK/Scene/Collision.cpp
    RSDKv5/RSDK/Scene/Object.cpp
    RSDKv5/RSDK/Scene/Objects/DefaultObject.cpp
    RSDKv5/RSDK/Scene/Objects/DevOutput.cpp
    RSDKv5/RSDK/User/Core/UserAchievements.cpp
    RSDKv5/RSDK/User/Core/UserCore.cpp
    RSDKv5/RSDK/User/Core/UserLeaderboards.cpp
    RSDKv5/RSDK/User/Core/UserPresence.cpp
    RSDKv5/RSDK/User/Core/UserStats.cpp
    RSDKv5/RSDK/User/Core/UserStorage.cpp
    dependencies/all/tinyxml2/tinyxml2.cpp
    dependencies/all/iniparser/iniparser.cpp
    dependencies/all/iniparser/dictionary.cpp
    dependencies/all/miniz/miniz.c
    SonicMania/Game.c
    SonicMania/Objects/All.c
    
    VideoVP9/ivfdec.cpp
    VideoVP9/tools_common.c
    VideoVP9/video_reader.cpp
    VideoVP9/vpx_config.c
)

if(NOT PLATFORM)
    if(WIN32) # THIS ASSUMES VCPKG OR SOURCES !!!!!!!
        set(PLATFORM "Windows" CACHE STRING "The platform to compile for.")
    elseif(ANDROID)
        set(PLATFORM "Android" CACHE STRING "The platform to compile for.")
    else()
        set(PLATFORM ${CMAKE_SYSTEM_NAME} CACHE STRING "The platform to compile for.")
    endif()
endif()

include(platforms/${PLATFORM}.cmake)

set_target_properties(RetroEngine PROPERTIES OUTPUT_NAME ${RETRO_OUTPUT_NAME})

target_include_directories(RetroEngine PRIVATE
    RSDKv5/
    dependencies/all/
    dependencies/all/tinyxml2/
    dependencies/all/iniparser/
	SonicMania/
	SonicMania/Objects
    
	VideoVP9
    dependencies/windows/libs_x86/include
    dependencies/windows/libs_x86/include/opus
)

target_link_directories(RetroEngine PRIVATE dependencies/windows/libs_x86/lib)
target_link_libraries(RetroEngine vpx opusfile opus ogg)

if(DEFINED DEP_PATH)
    target_include_directories(RetroEngine PRIVATE
        dependencies/${DEP_PATH}/
    )
endif()

if(NOT DEFINED GAME_STATIC)
    set(GAME_STATIC OFF)
endif()

if(DEFINED RETRO_EMBEDDED_DATA)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DRETRO_EMBEDDED_DATA")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DRETRO_EMBEDDED_DATA")
endif()

target_compile_definitions(RetroEngine PRIVATE
    RETRO_REVISION=${RETRO_REVISION}
    RSDK_USE_${RETRO_SUBSYSTEM}=1
    RETRO_USE_MOD_LOADER=$<BOOL:${RETRO_MOD_LOADER}>
    RETRO_STANDALONE=$<NOT:$<BOOL:${GAME_STATIC}>>
    RSDK_AUTOBUILD=$<BOOL:${RETRO_DISABLE_PLUS}>
    RETRO_DEV_EXTRA="${PLATFORM} - ${RETRO_SUBSYSTEM} - ${CMAKE_CXX_COMPILER_ID}"
    DECOMP_VERSION="${DECOMP_VERSION}"
)

if(NOT GAME_STATIC AND WITH_RSDK)
    add_custom_command(TARGET RetroEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:${GAME_NAME}>
        $<TARGET_FILE_DIR:RetroEngine>)
endif()