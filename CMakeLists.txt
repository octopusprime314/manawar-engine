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
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/drawbuffers")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/drawbuffers")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/engine")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/engine")
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
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/effects")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/effects")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/texture")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/texture")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/io")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/io")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/lights")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/lights")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/view")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/view")
endif()
if (NOT IS_DIRECTORY "${CMAKE_SOURCE_DIR}/dx")
    message(WARNING "Missing directory ${CMAKE_SOURCE_DIR}/dx")
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/ReBoot-dist")

include_directories("${CMAKE_SOURCE_DIR}/libs/glfw/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/fbx-sdk/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/freeimage/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/fmod/lowlevel/inc")
include_directories("${CMAKE_SOURCE_DIR}/model/include")
include_directories("${CMAKE_SOURCE_DIR}/gl/include")
include_directories("${CMAKE_SOURCE_DIR}/physics/include")
include_directories("${CMAKE_SOURCE_DIR}/shading/include")
include_directories("${CMAKE_SOURCE_DIR}/audio/include")
include_directories("${CMAKE_SOURCE_DIR}/drawbuffers/include")
include_directories("${CMAKE_SOURCE_DIR}/engine/include")
include_directories("${CMAKE_SOURCE_DIR}/math/include")
include_directories("${CMAKE_SOURCE_DIR}/procedural/include")
include_directories("${CMAKE_SOURCE_DIR}/renderer/include")
include_directories("${CMAKE_SOURCE_DIR}/effects/include")
include_directories("${CMAKE_SOURCE_DIR}/texture/include")
include_directories("${CMAKE_SOURCE_DIR}/io/include")
include_directories("${CMAKE_SOURCE_DIR}/lights/include")
include_directories("${CMAKE_SOURCE_DIR}/view/include")
include_directories("${CMAKE_SOURCE_DIR}/dx/include")

FILE(GLOB MODEL_HEADER_FILES        ${CMAKE_SOURCE_DIR}/model/include/*.h)
FILE(GLOB PHYSICS_HEADER_FILES      ${CMAKE_SOURCE_DIR}/physics/include/*.h)
FILE(GLOB SHADING_HEADER_FILES      ${CMAKE_SOURCE_DIR}/shading/include/*.h)
FILE(GLOB AUDIO_HEADER_FILES        ${CMAKE_SOURCE_DIR}/audio/include/*.h)
FILE(GLOB DRAWBUFFERS_HEADER_FILES  ${CMAKE_SOURCE_DIR}/drawbuffers/include/*.h)
FILE(GLOB GL_HEADER_FILES           ${CMAKE_SOURCE_DIR}/gl/include/*.h)
FILE(GLOB ENGINE_HEADER_FILES       ${CMAKE_SOURCE_DIR}/engine/include/*.h)
FILE(GLOB MATH_HEADER_FILES         ${CMAKE_SOURCE_DIR}/math/include/*.h)
FILE(GLOB PROCEDURAL_HEADER_FILES   ${CMAKE_SOURCE_DIR}/procedural/include/*.h)
FILE(GLOB RENDERER_HEADER_FILES     ${CMAKE_SOURCE_DIR}/renderer/include/*.h)
FILE(GLOB EFFECTS_HEADER_FILES      ${CMAKE_SOURCE_DIR}/effects/include/*.h)
FILE(GLOB TEXTURE_HEADER_FILES      ${CMAKE_SOURCE_DIR}/texture/include/*.h)
FILE(GLOB IO_HEADER_FILES 		    ${CMAKE_SOURCE_DIR}/io/include/*.h)
FILE(GLOB LIGHTS_HEADER_FILES 	    ${CMAKE_SOURCE_DIR}/lights/include/*.h)
FILE(GLOB GLSL_SHADER_FILES 	   	${CMAKE_SOURCE_DIR}/shading/shaders/glsl/*)
FILE(GLOB HLSL_SHADER_FILES 	   	${CMAKE_SOURCE_DIR}/shading/shaders/hlsl/*)
FILE(GLOB VIEW_HEADER_FILES 	    ${CMAKE_SOURCE_DIR}/view/include/*.h)
FILE(GLOB DX_HEADER_FILES 	     	${CMAKE_SOURCE_DIR}/dx/include/*.h)

FILE(GLOB MODEL_SRC_FILES           ${CMAKE_SOURCE_DIR}/model/src/*.cpp)
FILE(GLOB PHYSICS_SRC_FILES         ${CMAKE_SOURCE_DIR}/physics/src/*.cpp)
FILE(GLOB SHADING_SRC_FILES         ${CMAKE_SOURCE_DIR}/shading/src/*.cpp)
FILE(GLOB GL_SRC_FILES    	        ${CMAKE_SOURCE_DIR}/gl/src/*)
FILE(GLOB AUDIO_SRC_FILES           ${CMAKE_SOURCE_DIR}/audio/src/*.cpp)
FILE(GLOB DRAWBUFFERS_SRC_FILES     ${CMAKE_SOURCE_DIR}/drawbuffers/src/*.cpp)
FILE(GLOB ENGINE_SRC_FILES          ${CMAKE_SOURCE_DIR}/engine/src/*.cpp)
FILE(GLOB MATH_SRC_FILES            ${CMAKE_SOURCE_DIR}/math/src/*.cpp)
FILE(GLOB PROCEDURAL_SRC_FILES      ${CMAKE_SOURCE_DIR}/procedural/src/*.cpp)
FILE(GLOB RENDERER_SRC_FILES        ${CMAKE_SOURCE_DIR}/renderer/src/*.cpp)
FILE(GLOB EFFECTS_SRC_FILES         ${CMAKE_SOURCE_DIR}/effects/src/*.cpp)
FILE(GLOB TEXTURE_SRC_FILES         ${CMAKE_SOURCE_DIR}/texture/src/*.cpp)
FILE(GLOB IO_SRC_FILES              ${CMAKE_SOURCE_DIR}/io/src/*.cpp)
FILE(GLOB LIGHTS_SRC_FILES          ${CMAKE_SOURCE_DIR}/lights/src/*.cpp)
FILE(GLOB VIEW_SRC_FILES            ${CMAKE_SOURCE_DIR}/view/src/*.cpp)
FILE(GLOB DX_SRC_FILES              ${CMAKE_SOURCE_DIR}/dx/src/*.cpp)

source_group("model"         FILES 	${MODEL_HEADER_FILES} ${MODEL_SRC_FILES})
source_group("physics"       FILES 	${PHYSICS_HEADER_FILES} ${PHYSICS_SRC_FILES})
source_group("shading"       FILES 	${SHADING_HEADER_FILES} ${SHADING_SRC_FILES})
source_group("gl"            FILES 	${GL_HEADER_FILES} ${GL_SRC_FILES})
source_group("audio"         FILES 	${AUDIO_HEADER_FILES} ${AUDIO_SRC_FILES})
source_group("drawbuffers"   FILES 	${DRAWBUFFERS_HEADER_FILES} ${DRAWBUFFERS_SRC_FILES})
source_group("engine"        FILES 	${ENGINE_HEADER_FILES} ${ENGINE_SRC_FILES})
source_group("math"          FILES 	${MATH_HEADER_FILES} ${MATH_SRC_FILES})
source_group("procedural"    FILES 	${PROCEDURAL_HEADER_FILES} ${PROCEDURAL_SRC_FILES})
source_group("renderer"      FILES 	${RENDERER_HEADER_FILES} ${RENDERER_SRC_FILES})
source_group("effects"       FILES 	${EFFECTS_HEADER_FILES} ${EFFECTS_SRC_FILES})
source_group("texture"       FILES 	${TEXTURE_HEADER_FILES} ${TEXTURE_SRC_FILES})
source_group("io"            FILES 	${IO_HEADER_FILES} ${IO_SRC_FILES})
source_group("lights"        FILES 	${LIGHTS_HEADER_FILES} ${LIGHTS_SRC_FILES})
source_group("shaders/glsl"  FILES 	${GLSL_SHADER_FILES})
source_group("shaders/hlsl"  FILES 	${HLSL_SHADER_FILES})
source_group("view"          FILES 	${VIEW_HEADER_FILES} ${VIEW_SRC_FILES})
source_group("dx"            FILES 	${DX_HEADER_FILES} ${DX_SRC_FILES})

add_executable(ReBoot
    # WIN32
                ${MODEL_SRC_FILES}
                ${PHYSICS_SRC_FILES}
                ${SHADING_SRC_FILES}
                ${GL_HEADER_FILES}
				${GL_SRC_FILES}
                ${AUDIO_SRC_FILES}
                ${MODEL_HEADER_FILES}
                ${PHYSICS_HEADER_FILES}
                ${SHADING_HEADER_FILES}
                ${AUDIO_HEADER_FILES}
				${DRAWBUFFERS_HEADER_FILES} 
				${DRAWBUFFERS_SRC_FILES}
				${ENGINE_HEADER_FILES}
				${ENGINE_SRC_FILES}
				${MATH_HEADER_FILES}
				${MATH_SRC_FILES}
				${PROCEDURAL_HEADER_FILES}
				${PROCEDURAL_SRC_FILES}
				${RENDERER_HEADER_FILES} 
				${RENDERER_SRC_FILES}
				${EFFECTS_HEADER_FILES} 
				${EFFECTS_SRC_FILES}
				${TEXTURE_HEADER_FILES}
				${TEXTURE_SRC_FILES}
				${IO_HEADER_FILES}
				${IO_SRC_FILES}
				${LIGHTS_HEADER_FILES}
				${LIGHTS_SRC_FILES}
				${GLSL_SHADER_FILES}
				${HLSL_SHADER_FILES}
				${VIEW_HEADER_FILES}
				${VIEW_SRC_FILES}
				${DX_HEADER_FILES}
				${DX_SRC_FILES})

target_compile_features(ReBoot PRIVATE cxx_range_for)

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions("/MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")
endif()

target_link_libraries(ReBoot           legacy_stdio_definitions.lib)
target_link_libraries(ReBoot           ${CMAKE_SOURCE_DIR}/libs/FreeImage/lib/FreeImage.lib)
target_link_libraries(ReBoot           ${CMAKE_SOURCE_DIR}/libs/glfw/lib/glfw3.lib)
target_link_libraries(ReBoot debug     ${CMAKE_SOURCE_DIR}/libs/fbx-sdk/lib/debug/libfbxsdk-md.lib)
target_link_libraries(ReBoot optimized ${CMAKE_SOURCE_DIR}/libs/fbx-sdk/lib/release/libfbxsdk-md.lib)
target_link_libraries(ReBoot           ${CMAKE_SOURCE_DIR}/libs/fmod/lowlevel/lib/fmod64_vc.lib)

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
