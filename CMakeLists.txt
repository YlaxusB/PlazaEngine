cmake_minimum_required(VERSION 3.21)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>")

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")
endif()
set(TARGET_BUILD_PLATFORM "windows")
set(PLATFORM "Windows")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_DEBUG /MDd")


project(PlazaEngine)
set(CMAKE_CXX_STANDARD 23)

set(PX_BUILDSNIPPETS OFF CACHE BOOL "Generate the snippets")
set(PX_BUILDPUBLICSAMPLES OFF CACHE BOOL "Generate the samples projects")
#set(PX_GENERATE_STATIC_LIBRARIES ON CACHE BOOL "Generate static libraries")
set(PLATFORM_CMAKELISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
set(PHYSX_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx")
set(PHYSX_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(PX_OUTPUT_LIB_DIR "${CMAKE_CURRENT_BINARY_DIR}/out")
set(PX_OUTPUT_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/out")

set(PHYSX_SDK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx")
set(PHYSX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx/source")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FS")
add_compile_definitions(_DEBUG)
add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)
option(PHYSX_BUILD_TYPE "debug")
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CURRENT_LIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/assimp/")
set(CMAKE_CURRENT_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/assimp/")

set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)

add_subdirectory("ThirdParty/assimp/")
add_subdirectory("ThirdParty/AL/")
add_subdirectory("ThirdParty/freetype/")
add_subdirectory("ThirdParty/GLFW/")
#add_subdirectory("ThirdParty/mono/")
add_subdirectory("ThirdParty/Tracy/")
add_subdirectory("ThirdParty/yaml/")
include("ThirdParty/PhysX/physx/compiler/vc17win64/cmake_install.cmake")
#add_subdirectory("ThirdParty/PhysX/physx/compiler/public/")
#add_subdirectory("ThirdParty/PhysX/physx/bin/win.x86_64.vc143.mt/debug/")
#add_subdirectory(libs)

#include("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx/compiler/public/CMakeLists.txt")

#include_directories("${PHYSX_SDK_PATH}/include")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/debug")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/compiler/public")
#set(PHYSX_BIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx/bin/win.x86_64.vc143.mt/debug")
set(PHYSX_SLN_PHYSXDEVICE_PATH "${PHYSX_BIN_FILES}")
#add_executable(PlazaEngine "Engine/Application/EntryPoint.cpp")



#file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Editor/*.cpp")
file(GLOB_RECURSE MY_SOURCES2 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.cpp")
file(GLOB_RECURSE MY_SOURCES3 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/*.h")
file(GLOB_RECURSE MY_SOURCES4 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/*.cpp")
file(GLOB_RECURSE MY_SOURCES5 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/*.hpp")

#file(GLOB_RECURSE IMGUI_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/*.cpp")
#file(GLOB_RECURSE IMGUI_SOURCES2 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/*.h")

set(IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/ImGuizmo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_impl_opengl3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp"
)

add_executable(PlazaEngine "${IMGUI_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Application/EntryPoint.cpp" "${MY_SOURCES}" "${MY_SOURCES2}")


#target_include_directories(PlazaEngine PRIVATE "${PHYSX_ROOT_DIR}/include" PRIVATE "${PHYSX_ROOT_DIR}/source/physxextensions/src")
#target_compile_definitions(PlazaEngine PRIVATE ${SNIPPET_COMPILE_DEFS})

target_compile_options(PlazaEngine PRIVATE /WX-)

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /C2220")  # Add the specific warning code you want to suppress
#add_subdirectory("Physx/physx/source/compiler/cmake")

#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/Physx/physx/compiler/public")

include_directories("./")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/AL/include/AL")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/assimp/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/filesystem/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/FileWathcer/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/freetype/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glad/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/GLFW/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/glm/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Json/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/KHR/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/minimp3/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx/include/")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx/source/physx/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/premake/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb/")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Tracy/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/uuid_v4/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vswhere/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/yaml/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/KHR/")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib/mono/")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib/")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx/bin/win.x86_64.vc143.mt/release/")
#C:\Users\Giovane\Desktop\Workspace\Plaza\ThirdParty\PhysX\physx\bin\win.x86_64.vc143.mt\release

set(PHYSX_LIBS 
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysX_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysXCooking_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysXCommon_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysXExtensions_static_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysXFoundation_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PVDRuntime_64.lib"
)
 
target_include_directories(PlazaEngine PUBLIC "ThirdParty/imgui/")
target_link_libraries(PlazaEngine PRIVATE glfw OpenAL freetype TracyClient yaml-cpp assimp libgcmonosgen mono-2.0-sgen MonoPosixHelper "${PHYSX_LIBS}" "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenAL32.lib")

# Set the path to the extracted PhysX SDK


# Add PhysX includes to your project
#include_directories(${PHYSX_SDK_PATH}/include)

# Add PhysX libraries to link against
#link_directories(${PHYSX_SDK_PATH}/bin/win.x86_64.vc143.mt/checked)

#target_include_directories(PlazaEngine)
#target_include_directories(PlazaEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/PhysX/physx")
#target_include_directories(PlazaEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Physx/physx/include")
#target_link_libraries(PlazaEngine PRIVATE imgui)