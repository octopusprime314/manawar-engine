cmake_minimum_required(VERSION 2.5)
project(manawar-engine)
set(VS_STARTUP_VERSION manawar-engine)
set(TLS_VERIFY OFF)

include(InstallRequiredSystemLibraries)

#grab and extract zip folder containing libs and assets
file(DOWNLOAD
	https://www.dropbox.com/s/9h8h3nvwk1gxyaw/data.zip?dl=0
	"${CMAKE_SOURCE_DIR}/data.zip"
	TIMEOUT 100  # seconds
    EXPECTED_HASH SHA1=36504e725d9c15f33a3ef9bc68367c62faa55513
    TLS_VERIFY ON
)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/manawar-engine-dist")

include_directories("${CMAKE_SOURCE_DIR}/libs/dx/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/glfw/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/fbx-sdk/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/freeimage/include")
include_directories("${CMAKE_SOURCE_DIR}/libs/fmod/inc")
include_directories("${CMAKE_SOURCE_DIR}/model/include")
include_directories("${CMAKE_SOURCE_DIR}/glLayer/include")
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
include_directories("${CMAKE_SOURCE_DIR}/dxLayer/include")

FILE(GLOB MODEL_HEADER_FILES        ${CMAKE_SOURCE_DIR}/model/include/*.h)
FILE(GLOB PHYSICS_HEADER_FILES      ${CMAKE_SOURCE_DIR}/physics/include/*.h)
FILE(GLOB SHADING_HEADER_FILES      ${CMAKE_SOURCE_DIR}/shading/include/*.h)
FILE(GLOB AUDIO_HEADER_FILES        ${CMAKE_SOURCE_DIR}/audio/include/*.h)
FILE(GLOB DRAWBUFFERS_HEADER_FILES  ${CMAKE_SOURCE_DIR}/drawbuffers/include/*.h)
FILE(GLOB GL_HEADER_FILES           ${CMAKE_SOURCE_DIR}/glLayer/include/*.h)
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
FILE(GLOB DX_HEADER_FILES 	     	${CMAKE_SOURCE_DIR}/dxLayer/include/*.h)

FILE(GLOB MODEL_SRC_FILES           ${CMAKE_SOURCE_DIR}/model/src/*.cpp)
FILE(GLOB PHYSICS_SRC_FILES         ${CMAKE_SOURCE_DIR}/physics/src/*.cpp)
FILE(GLOB SHADING_SRC_FILES         ${CMAKE_SOURCE_DIR}/shading/src/*.cpp)
FILE(GLOB GL_SRC_FILES    	        ${CMAKE_SOURCE_DIR}/glLayer/src/*)
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
FILE(GLOB DX_SRC_FILES              ${CMAKE_SOURCE_DIR}/dxLayer/src/*.cpp)

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

add_executable(manawar-engine
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

target_compile_features(manawar-engine PRIVATE cxx_range_for)

if (MSVC)
	set(VS_STARTUP_Project manawar-engine)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions("/MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
	set_source_files_properties(${HLSL_SHADER_FILES} PROPERTIES VS_TOOL_OVERRIDE "None")
endif()

target_link_libraries(manawar-engine           legacy_stdio_definitions.lib)
target_link_libraries(manawar-engine           ${CMAKE_SOURCE_DIR}/libs/FreeImage/lib/FreeImage.lib)
target_link_libraries(manawar-engine           ${CMAKE_SOURCE_DIR}/libs/glfw/lib/glfw3.lib)
target_link_libraries(manawar-engine debug     ${CMAKE_SOURCE_DIR}/libs/fbx-sdk/lib/debug/libfbxsdk-md.lib)
target_link_libraries(manawar-engine optimized ${CMAKE_SOURCE_DIR}/libs/fbx-sdk/lib/release/libfbxsdk-md.lib)
target_link_libraries(manawar-engine           ${CMAKE_SOURCE_DIR}/libs/fmod/lib/fmod64_vc.lib)


add_custom_command(TARGET manawar-engine PRE_BUILD
				   COMMAND ${CMAKE_COMMAND} -E tar xzf data.zip
				   WORKING_DIRECTORY ../
				   DEPENDS ${CMAKE_SOURCE_DIR}/data.zip
				   COMMENT "Unpacking data.zip"
				   VERBATIM)

#dxil and dxcompiler dlls are needed for dxr
add_custom_command(TARGET manawar-engine POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/libs/freeimage/lib/FreeImage.dll $<TARGET_FILE_DIR:manawar-engine>
				   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/libs/fmod/lib/fmod64.dll 		 $<TARGET_FILE_DIR:manawar-engine> 
				   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/libs/dx/lib/dxcompiler.dll       $<TARGET_FILE_DIR:manawar-engine>
				   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/libs/dx/lib/dxil.dll			 $<TARGET_FILE_DIR:manawar-engine>)

install(TARGETS manawar-engine RUNTIME DESTINATION bin)
install(FILES "${CMAKE_SOURCE_DIR}/libs/freeimage/lib/FreeImage.dll"
              "${CMAKE_SOURCE_DIR}/libs/fmod/lib/fmod64.dll"
              "${CMAKE_SOURCE_DIR}/libs/dx/lib/dxcompiler.dll"
        DESTINATION bin)
# FYI: Keep the trailing '/'s here.
install(DIRECTORY "${CMAKE_SOURCE_DIR}/shading/shaders/"
        DESTINATION shading/shaders)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/assets/"
        DESTINATION assets)
		
