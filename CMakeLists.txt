cmake_minimum_required(VERSION 2.5)
project(ReBoot)
set(VS_STARTUP_VERSION ReBoot)

include(InstallRequiredSystemLibraries)

if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/glfw")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/libs/glfw")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/fbx-sdk")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/libs/fbx-sdk")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/freeimage")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/libs/freeimage")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/model")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/model")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/physics")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/physics")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/shading")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/shading")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/libs/fmod")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/libs/fmod")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/buffers")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/buffers")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/events")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/events")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/math")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/math")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/procedural")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/procedural")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/renderer")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/renderer")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/screenspace")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/screenspace")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/texture")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/texture")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/io")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/io")
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/4fps-dist")

include_directories("${CMAKE_SOURCE_DIR}/libs/glfw/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/fbx-sdk/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/freeimage/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/fmod/lowlevel/inc")
include_directories("${CMAKE_SOURCE_DIR}/model/include")
include_directories("${CMAKE_SOURCE_DIR}/physics/include")
include_directories("${CMAKE_SOURCE_DIR}/shading/include")
include_directories("${CMAKE_SOURCE_DIR}/audio/include")
include_directories("${CMAKE_SOURCE_DIR}/buffers/include")
include_directories("${CMAKE_SOURCE_DIR}/events/include")
include_directories("${CMAKE_SOURCE_DIR}/math/include")
include_directories("${CMAKE_SOURCE_DIR}/procedural/include")
include_directories("${CMAKE_SOURCE_DIR}/renderer/include")
include_directories("${CMAKE_SOURCE_DIR}/screenspace/include")
include_directories("${CMAKE_SOURCE_DIR}/texture/include")
include_directories("${CMAKE_SOURCE_DIR}/io/include")

FILE(GLOB MODEL_HEADER_FILES ${CMAKE_SOURCE_DIR}/model/include/*.h)
FILE(GLOB PHYSICS_HEADER_FILES ${CMAKE_SOURCE_DIR}/physics/include/*.h)
FILE(GLOB SHADING_HEADER_FILES ${CMAKE_SOURCE_DIR}/shading/include/*.h)
FILE(GLOB AUDIO_HEADER_FILES ${CMAKE_SOURCE_DIR}/audio/include/*.h)
FILE(GLOB BUFFERS_HEADER_FILES ${CMAKE_SOURCE_DIR}/buffers/include/*.h)
FILE(GLOB EVENTS_HEADER_FILES ${CMAKE_SOURCE_DIR}/events/include/*.h)
FILE(GLOB MATH_HEADER_FILES ${CMAKE_SOURCE_DIR}/math/include/*.h)
FILE(GLOB PROCEDURAL_HEADER_FILES ${CMAKE_SOURCE_DIR}/procedural/include/*.h)
FILE(GLOB RENDERER_HEADER_FILES ${CMAKE_SOURCE_DIR}/renderer/include/*.h)
FILE(GLOB SCREENSPACE_HEADER_FILES ${CMAKE_SOURCE_DIR}/screenspace/include/*.h)
FILE(GLOB TEXTURE_HEADER_FILES ${CMAKE_SOURCE_DIR}/texture/include/*.h)
FILE(GLOB IO_HEADER_FILES ${CMAKE_SOURCE_DIR}/io/include/*.h)

FILE(GLOB MODEL_SRC_FILES ${CMAKE_SOURCE_DIR}/model/src/*.cpp)
FILE(GLOB PHYSICS_SRC_FILES ${CMAKE_SOURCE_DIR}/physics/src/*.cpp)
FILE(GLOB SHADING_SRC_FILES ${CMAKE_SOURCE_DIR}/shading/src/*.cpp)
FILE(GLOB GLFWPROC_SRC_FILES ${CMAKE_SOURCE_DIR}/glfw/src/*.c)
FILE(GLOB AUDIO_SRC_FILES ${CMAKE_SOURCE_DIR}/audio/src/*.cpp)
FILE(GLOB BUFFERS_SRC_FILES ${CMAKE_SOURCE_DIR}/buffers/src/*.cpp)
FILE(GLOB EVENTS_SRC_FILES ${CMAKE_SOURCE_DIR}/events/src/*.cpp)
FILE(GLOB MATH_SRC_FILES ${CMAKE_SOURCE_DIR}/math/src/*.cpp)
FILE(GLOB PROCEDURAL_SRC_FILES ${CMAKE_SOURCE_DIR}/procedural/src/*.cpp)
FILE(GLOB RENDERER_SRC_FILES ${CMAKE_SOURCE_DIR}/renderer/src/*.cpp)
FILE(GLOB SCREENSPACE_SRC_FILES ${CMAKE_SOURCE_DIR}/screenspace/src/*.cpp)
FILE(GLOB TEXTURE_SRC_FILES ${CMAKE_SOURCE_DIR}/texture/src/*.cpp)
FILE(GLOB IO_SRC_FILES ${CMAKE_SOURCE_DIR}/io/src/*.cpp)

source_group("model"   FILES ${MODEL_HEADER_FILES} ${MODEL_SRC_FILES})
source_group("physics" FILES ${PHYSICS_HEADER_FILES} ${PHYSICS_SRC_FILES})
source_group("shading" FILES ${SHADING_HEADER_FILES} ${SHADING_SRC_FILES})
source_group("glfw"    FILES ${GLFWPROC_SRC_FILES})
source_group("audio"   FILES ${AUDIO_HEADER_FILES} ${AUDIO_SRC_FILES})
source_group("buffers"   FILES ${BUFFERS_HEADER_FILES} ${BUFFERS_SRC_FILES})
source_group("events" FILES ${EVENTS_HEADER_FILES} ${EVENTS_SRC_FILES})
source_group("math" FILES ${MATH_HEADER_FILES} ${MATH_SRC_FILES})
source_group("procedural"    FILES ${PROCEDURAL_HEADER_FILES} ${PROCEDURAL_SRC_FILES})
source_group("renderer"   FILES ${RENDERER_HEADER_FILES} ${RENDERER_SRC_FILES})
source_group("screenspace"    FILES ${SCREENSPACE_HEADER_FILES} ${SCREENSPACE_SRC_FILES})
source_group("texture"   FILES ${TEXTURE_HEADER_FILES} ${TEXTURE_SRC_FILES})
source_group("io"   FILES ${IO_HEADER_FILES} ${IO_SRC_FILES})

add_executable(ReBoot
    # WIN32
                ${MODEL_SRC_FILES}
                ${PHYSICS_SRC_FILES}
                ${SHADING_SRC_FILES}
                ${GLFWPROC_SRC_FILES}
                ${AUDIO_SRC_FILES}
                ${MODEL_HEADER_FILES}
                ${PHYSICS_HEADER_FILES}
                ${SHADING_HEADER_FILES}
                ${AUDIO_HEADER_FILES}
				${BUFFERS_HEADER_FILES} 
				${BUFFERS_SRC_FILES}
				${EVENTS_HEADER_FILES}
				${EVENTS_SRC_FILES}
				${MATH_HEADER_FILES}
				${MATH_SRC_FILES}
				${PROCEDURAL_HEADER_FILES}
				${PROCEDURAL_SRC_FILES}
				${RENDERER_HEADER_FILES} 
				${RENDERER_SRC_FILES}
				${SCREENSPACE_HEADER_FILES} 
				${SCREENSPACE_SRC_FILES}
				${TEXTURE_HEADER_FILES}
				${TEXTURE_SRC_FILES}
				${IO_HEADER_FILES}
				${IO_SRC_FILES})

target_compile_features(ReBoot PRIVATE cxx_range_for)

if (MSVC)
    add_definitions("/MP")
endif()

target_link_libraries(ReBoot legacy_stdio_definitions.lib)
target_link_libraries(ReBoot ${CMAKE_SOURCE_DIR}/libs/FreeImage/lib/FreeImage.lib)
target_link_libraries(ReBoot ${CMAKE_SOURCE_DIR}/libs/glfw/lib/glfw3.lib)
target_link_libraries(ReBoot debug     ${CMAKE_SOURCE_DIR}/libs/fbx-sdk/lib/debug/libfbxsdk-md.lib)
target_link_libraries(ReBoot optimized ${CMAKE_SOURCE_DIR}/libs/fbx-sdk/lib/release/libfbxsdk-md.lib)
target_link_libraries(ReBoot ${CMAKE_SOURCE_DIR}/libs/fmod/lowlevel/lib/fmod64_vc.lib)

install(TARGETS ReBoot RUNTIME DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/libs/freeimage/lib/FreeImage.dll"
              "${CMAKE_SOURCE_DIR}/libs/fmod/lowlevel/lib/fmod64.dll"
              "${CMAKE_SOURCE_DIR}/libs/fmod/lowlevel/lib/fmodL64.dll"
        DESTINATION bin)
# FYI: Keep the trailing '/'s here.
install(DIRECTORY "${CMAKE_SOURCE_DIR}/shading/shaders/"
        DESTINATION shading/shaders)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/"
        DESTINATION assets)
