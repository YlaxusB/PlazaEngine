cmake_minimum_required(VERSION 3.21)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug$<$<CONFIG:Debug>:Debug>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")
endif()
set(TARGET_BUILD_PLATFORM "windows")
set(PLATFORM "Windows")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG /MDd")

SET(SNIPPET_COMPILE_DEFS
	# Common to all configurations

	${PHYSX_WINDOWS_COMPILE_DEFS};RENDER_SNIPPET;${PHYSX_LIBTYPE_DEFS};${PHYSXGPU_LIBTYPE_DEFS}

	$<$<CONFIG:debug>:${PHYSX_WINDOWS_DEBUG_COMPILE_DEFS};>
	$<$<CONFIG:checked>:${PHYSX_WINDOWS_CHECKED_COMPILE_DEFS};>
	$<$<CONFIG:profile>:${PHYSX_WINDOWS_PROFILE_COMPILE_DEFS};>
	$<$<CONFIG:release>:${PHYSX_WINDOWS_RELEASE_COMPILE_DEFS};>
)

# Add Tracy as a subdirectory
add_subdirectory("ThirdParty/Tracy/")


project(PlazaEngine)
set(CMAKE_CXX_STANDARD 20)

set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")
set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
#set(PX_GENERATE_STATIC_LIBRARIES ON CACHE BOOL "Generate static libraries")
set(PLATFORM_CMAKELISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
set(PHYSX_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Physx/physx")
set(PHYSX_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Physx/physx")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(PX_OUTPUT_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/physx")
set(PX_OUTPUT_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/ThirdParty/physx")

set(PHYSX_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx")


option(PHYSX_BUILD_TYPE "debug")

    add_compile_definitions(_DEBUG)

#include_directories("${PHYSX_SDK_PATH}/include")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/debug")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/compiler/public")
#set(PHYSX_BIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/debug")
set(PHYSX_SLN_PHYSXDEVICE_PATH "${PHYSX_BIN_FILES}")
#add_executable(PlazaEngine "Engine/Application/EntryPoint.cpp")



#file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Editor/*.cpp")
file(GLOB_RECURSE MY_SOURCES2 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.cpp")

add_executable(PlazaEngine "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Application/EntryPoint.cpp" "${MY_SOURCES}" "${MY_SOURCES2}")


target_include_directories(PlazaEngine PRIVATE "${PHYSX_ROOT_DIR}/include" PRIVATE "${PHYSX_ROOT_DIR}/source/physxextensions/src")
target_compile_definitions(PlazaEngine PRIVATE ${SNIPPET_COMPILE_DEFS})

target_precompile_headers(PlazaEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core/PreCompiledHeaders.h")

#Physx

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Physx/physx/source/compiler/cmake/")
set( PHYSX_LIBRARIES
	PhysX
	PhysXExtensions
	PhysXPvdSDK
	PhysXVehicle
	PhysXCharacterKinematic
	PhysXCooking
	PhysXCommon
	PhysXFoundation
	# SnippetUtils
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244")  # Add the specific warning code you want to suppress
#add_subdirectory("Physx/physx/source/compiler/cmake")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Physx/physx/compiler/public")

include_directories("./")
include_directories("ThirdParty/")
include_directories("ThirdParty/AL")
include_directories("ThirdParty/filesystem/")
include_directories("ThirdParty/FileWathcer/")
include_directories("ThirdParty/freetype/")
include_directories("ThirdParty/glad/")
include_directories("ThirdParty/GLFW/")
include_directories("ThirdParty/glm/")
include_directories("ThirdParty/imgui/")
include_directories("ThirdParty/Json/")
include_directories("ThirdParty/KHR/")
include_directories("ThirdParty/minimp3/")
include_directories("ThirdParty/mono/")
include_directories("ThirdParty/physx/")
include_directories("ThirdParty/premake/")
include_directories("ThirdParty/stb/")
include_directories("ThirdParty/Tracy/")
include_directories("ThirdParty/uuid_v4/")
include_directories("ThirdParty/vswhere/")
include_directories("ThirdParty/yaml/include")

# Set the path to the extracted PhysX SDK


# Add PhysX includes to your project
#include_directories(${PHYSX_SDK_PATH}/include)

# Add PhysX libraries to link against
#link_directories(${PHYSX_SDK_PATH}/bin/win.x86_64.vc143.mt/checked)

target_include_directories(PlazaEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Tracy/")
#target_include_directories(PlazaEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx")
#target_include_directories(PlazaEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Physx/physx/include")

