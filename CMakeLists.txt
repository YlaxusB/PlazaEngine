cmake_minimum_required(VERSION 3.21)


#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:MultiThreaded>$<$<CONFIG:GameRelease>:MultiThreaded>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(MSVC)
    add_compile_options(/MP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG /MDd /ZI /Ob0 /Od /RTC1")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /O2")


# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(TARGET_BUILD_PLATFORM "windows")
set(PLATFORM "Windows")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(PlazaEngine)
#set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
#set(CMAKE_CXX_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.39.33519/bin/Hostx64/x64/cl.exe")
#set(CMAKE_GENERATOR "Visual Studio 17 2022" CACHE OFF "" FORCE)

include(FindVulkan)
if(NOT VULKAN_FOUND)
    message(FATAL_ERROR "Vulkan SDK not installed.")
endif()
find_package(Vulkan REQUIRED)

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
set(ASSIMP_BUILD_STATIC_LIB ON)
set(GLM_FORCE_DEPTH_ZERO_TO_ONE ON)

add_subdirectory("ThirdParty/AL/")
add_subdirectory("ThirdParty/DirectXTex/")
add_subdirectory("ThirdParty/freetype/")
add_subdirectory("ThirdParty/GLFW/")
add_subdirectory("ThirdParty/Tracy/")
add_subdirectory("ThirdParty/yaml-cpp/")
#add_subdirectory("ThirdParty/Physx/physx/compiler/public/")
include("ThirdParty/PhysX/physx/compiler/vc17win64/cmake_install.cmake")
#include("ThirdParty/PhysX/physx/compiler/vc17win64/cmake_install.cmake")

#add_subdirectory("ThirdParty/PhysX/physx/compiler/vc17win64/")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib/mono/")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/../lib/")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx/bin/win.x86_64.vc143.mt/release/")
#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/")

#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Physx/physx/bin/win.x86_64.vc143.mt/debug/")
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/lib/Debug/")

#set(PHYSX_SLN_PHYSXDEVICE_PATH "${PHYSX_BIN_FILES}")

file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Editor/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core/Renderer/stb_font_consolas_24_latin1.inl")
file(GLOB_RECURSE MY_SOURCES2 CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Engine/*.cpp")

set(IMGUI_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_demo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_widgets.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/ImGuizmo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_impl_opengl3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_impl_glfw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_impl_vulkan.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/imgui/imgui_tables.cpp"
)

add_executable(PlazaEngine "${IMGUI_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Application/EntryPoint.cpp" "${MY_SOURCES}" "${MY_SOURCES2}" "Editor/GUI/GuiWindow.h")

add_executable(PlazaShippingGame "${IMGUI_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Application/EntryPoint.cpp" "${MY_SOURCES}" "${MY_SOURCES2}" "Editor/GUI/GuiWindow.h")

target_compile_definitions(PlazaEngine PRIVATE EDITOR_MODE)

target_compile_definitions(PlazaShippingGame PRIVATE GAME_MODE)

target_compile_options(PlazaEngine PRIVATE /WX-)
target_compile_options(PlazaShippingGame PRIVATE /WX-)

target_precompile_headers(PlazaEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core/PreCompiledHeaders.h")
target_precompile_headers(PlazaShippingGame PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core/PreCompiledHeaders.h")

if(MSVC)
  target_compile_options(PlazaEngine PUBLIC "/ZI")
  target_link_options(PlazaEngine PUBLIC "/INCREMENTAL")

    target_compile_options(PlazaShippingGame PUBLIC "/ZI")
  target_link_options(PlazaShippingGame PUBLIC "/INCREMENTAL")
endif()

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
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/cereal/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/DirectXTex/DirectXTex/")
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
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx/include/")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/premake/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/stb/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyobjloader/")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/Tracy/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ufbx/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/uuid_v4/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vswhere/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vulkan/vulkan/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/yaml/include/yaml-cpp/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/KHR/")
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/OpenFBX/src/")



#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx/compiler/public")

#C:\Users\Giovane\Desktop\Workspace\Plaza\ThirdParty\PhysX\physx\bin\win.x86_64.vc143.mt\release

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug")

set(PHYSX_LIBS 
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysX_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysXCooking_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysXCommon_64.lib"
#"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysXExtensions_static_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysXFoundation_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PVDRuntime_64.lib"
"${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release/PhysXCharacterKinematic_static_64.lib"
)

set(OPENAL_DLL "${BUILD_DIR}/ThirdParty/AL/OpenAL32d.dll")
set(FREETYPE_DLL "${BUILD_DIR}/ThirdParty/freetype/freetyped.dll")
set(GLFW_DLL "${BUILD_DIR}/ThirdParty/GLFW/src/glfw3d.dll")
set(TRACY_DLL "${BUILD_DIR}/ThirdParty/Tracy/TracyClientd.dll")
set(YAML_DLL "${BUILD_DIR}/ThirdParty/yaml/yaml-cppd.dll")

#set(PHYSX_DLL_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release")
set(PHYSX_DLL_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/dll")

set(PHYSX_DLLS 
#"PhysX_64.dll"
"PhysXCooking_64.dll"
"PhysXCommon_64.dll"
"PhysXFoundation_64.dll"
)
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

target_include_directories(PlazaEngine PUBLIC "lib/")
target_include_directories(PlazaShippingGame PUBLIC "lib/")

if(MSVC)
    add_compile_definitions(_ITERATOR_DEBUG_LEVEL=2)  # Disable iterator debugging
endif()

target_include_directories(PlazaEngine PUBLIC "ThirdParty/imgui/")
target_link_libraries(PlazaEngine PRIVATE glfw OpenAL freetype TracyClient yaml-cpp mono-2.0-sgen MonoPosixHelper DirectXTex
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysX_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysXCommon_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysXFoundation_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysXCooking_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysXExtensions_static_64.lib"
 "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenAL32.lib" Vulkan::Vulkan)

target_include_directories(PlazaShippingGame PUBLIC "ThirdParty/imgui/")
target_link_libraries(PlazaShippingGame PRIVATE glfw OpenAL freetype TracyClient yaml-cpp mono-2.0-sgen MonoPosixHelper DirectXTex
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysX_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysXCommon_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysXFoundation_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysXCooking_64.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug/PhysXExtensions_static_64.lib"
 "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenAL32.lib" Vulkan::Vulkan)