cmake_minimum_required(VERSION 2.5)
project(ReBoot)

set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
 
include_directories("${CMAKE_BINARY_DIR}/../libs/glfw/include")
include_directories("${CMAKE_BINARY_DIR}/../libs/fbx-sdk/include")
include_directories("${CMAKE_BINARY_DIR}/../libs/freeimage/include")
include_directories("${CMAKE_BINARY_DIR}/../model/include")
include_directories("${CMAKE_BINARY_DIR}/../physics/include")
include_directories("${CMAKE_BINARY_DIR}/../shading/include")

FILE(GLOB MODEL_HEADER_FILES ${CMAKE_BINARY_DIR}/../model/include/*.h)
FILE(GLOB PHYSICS_HEADER_FILES ${CMAKE_BINARY_DIR}/../physics/include/*.h)
FILE(GLOB SHADING_HEADER_FILES ${CMAKE_BINARY_DIR}/../shading/include/*.h)
FILE(GLOB MODEL_SRC_FILES ${CMAKE_BINARY_DIR}/../model/src/*.cpp)
FILE(GLOB PHYSICS_SRC_FILES ${CMAKE_BINARY_DIR}/../physics/src/*.cpp)
FILE(GLOB SHADING_SRC_FILES ${CMAKE_BINARY_DIR}/../shading/src/*.cpp)
FILE(GLOB GLFWPROC_SRC_FILES ${CMAKE_BINARY_DIR}/../glfw/src/*.c)

source_group("model" FILES ${MODEL_HEADER_FILES} ${MODEL_SRC_FILES})
source_group("physics" FILES ${PHYSICS_HEADER_FILES} ${PHYSICS_SRC_FILES})
source_group("shading" FILES ${SHADING_HEADER_FILES} ${SHADING_SRC_FILES})
source_group("glfw" FILES ${GLFWPROC_SRC_FILES})

add_executable(ReBoot ${MODEL_SRC_FILES} ${PHYSICS_SRC_FILES} ${SHADING_SRC_FILES} ${GLFWPROC_SRC_FILES} ${MODEL_HEADER_FILES} ${PHYSICS_HEADER_FILES} ${SHADING_HEADER_FILES}) 

target_compile_features(ReBoot PRIVATE cxx_range_for)

target_link_libraries(ReBoot ${CMAKE_BINARY_DIR}/../libs/FreeImage/lib/FreeImage.lib)
target_link_libraries(ReBoot ${CMAKE_BINARY_DIR}/../libs/glfw/lib/glfw3.lib)
target_link_libraries(ReBoot debug ${CMAKE_BINARY_DIR}/../libs/fbx-sdk/lib/debug/libfbxsdk-md.lib)
target_link_libraries(ReBoot optimized ${CMAKE_BINARY_DIR}/../libs/fbx-sdk/lib/release/libfbxsdk-md.lib)
