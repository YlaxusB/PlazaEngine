cmake_minimum_required(VERSION 3.24)


#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:MultiThreaded>$<$<CONFIG:GameRelease>:MultiThreaded>")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(MSVC)
    add_compile_options(/MP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG /MDd /ZI /Ob0 /Od /RTC1")
endif()
add_compile_definitions(_DEBUG)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /O2")
set(BUILD_STATIC_LIB ON)

if(MSVC)
    add_compile_definitions(_ITERATOR_DEBUG_LEVEL=2)
endif()

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(TARGET_BUILD_PLATFORM "windows")
set(PLATFORM "Windows")

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

project(PlazaEngineProject)
#set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
#set(CMAKE_CXX_COMPILER "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.39.33519/bin/Hostx64/x64/cl.exe")

include(FindVulkan)
if(NOT VULKAN_FOUND)
    message(FATAL_ERROR "Vulkan SDK not installed.")
endif()
find_package(Vulkan REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

set(PLATFORM_CMAKELISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

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

set(PLAZA_ITERATOR_LEVEL 2)

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-Debug")
set(PHYSX_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx")
set(PX_OUTPUT_LIB_DIR "${BUILD_DIR}/libs")
set(PX_OUTPUT_BIN_DIR "${BUILD_DIR}/bin")

OPTION(COPY_EXTERNAL_DLLS "Control the copying of external DLLs" OFF) # PhysX external dlls
option(PX_BUILDSNIPPETS "Generate the snippets" OFF)
option(PX_BUILDPVDRUNTIME "Generate the OmniPVD project" ON)
option(PX_GENERATE_STATIC_LIBRARIES "Generate static libraries" ON)
option(NV_USE_STATIC_WINCRT "Use the statically linked windows CRT" ON)
option(NV_USE_DEBUG_WINCRT "Use the debug version of the CRT" ON)
option(PX_FLOAT_POINT_PRECISE_MATH "Float point precise math" OFF)
#include("${PHYSX_ROOT_DIR}/compiler/public/CMakeLists.txt")
OPTION(PX_BUILDSNIPPETS "Generate the snippets" OFF)
OPTION(PX_BUILDPVDRUNTIME "Generate the OmniPVD project" OFF)
OPTION(PX_CMAKE_SUPPRESS_REGENERATION "Disable zero_check projects" OFF)

include("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/PhysX/physx/compiler/public/CMakeLists.txt")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/lib/Debug/")

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

#add_dependencies(PlazaEngine PhysX5Solution)
#add_dependencies(PlazaShippingGame PhysX5Solution)

target_compile_definitions(PlazaEngine PRIVATE PX_PHYSX_STATIC_LIB)
target_compile_definitions(PlazaShippingGame PRIVATE PX_PHYSX_STATIC_LIB)

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

set(OPENAL_DLL "${BUILD_DIR}/ThirdParty/AL/OpenAL32.dll")
set(FREETYPE_DLL "${BUILD_DIR}/ThirdParty/freetype/freetyped.dll")
set(GLFW_DLL "${BUILD_DIR}/ThirdParty/GLFW/src/glfw3d.dll")
set(TRACY_DLL "${BUILD_DIR}/ThirdParty/Tracy/TracyClientd.dll")
set(YAML_DLL "${BUILD_DIR}/ThirdParty/yaml/yaml-cppd.dll")

#set(PHYSX_DLL_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/out/install/x64-Debug/bin/win.x86_64.vc143.mt/release")
set(PHYSX_DLL_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/dll")

set(PHYSX_DLLS 
"PhysXCooking_64.dll"
"PhysXCommon_64.dll"
"PhysXFoundation_64.dll"
)

set(PHYSX_LIBS
    #"${CMAKE_CURRENT_SOURCE_DIR}/lib/PhysX_64.lib"
    #"${CMAKE_CURRENT_SOURCE_DIR}/lib/PhysXCommon_64.lib"
    #"${CMAKE_CURRENT_SOURCE_DIR}/lib/PhysXFoundation_64.lib"
    #"${CMAKE_CURRENT_SOURCE_DIR}/lib/PhysXCooking_64.lib"
    #"${CMAKE_CURRENT_SOURCE_DIR}/lib/PhysXExtensions_static_64.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/debug/PhysX_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/debug/PhysXExtensions_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/debug/PhysXCooking_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/debug/PhysXCommon_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/debug/PhysXFoundation_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/debug/PhysXPvdSDK_static.lib"
)

set(PHYSX_DLLS
    "${CMAKE_CURRENT_SOURCE_DIR}/dll/PhysX_64.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/dll/PhysXCommon_64.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/dll/PhysXFoundation_64.dll"
    "${CMAKE_CURRENT_SOURCE_DIR}/dll/PhysXCooking_64.dll"
)

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

target_include_directories(PlazaEngine PUBLIC "lib/")
target_include_directories(PlazaShippingGame PUBLIC "lib/")

add_dependencies(PlazaEngine PhysX)
add_dependencies(PlazaShippingGame PhysX)

target_include_directories(PlazaEngine PUBLIC "ThirdParty/imgui/")
target_link_libraries(PlazaEngine PRIVATE glfw OpenAL freetype TracyClient yaml-cpp mono-2.0-sgen MonoPosixHelper DirectXTex ${PHYSX_LIBS} "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenAL32.lib" Vulkan::Vulkan)

target_include_directories(PlazaShippingGame PUBLIC "ThirdParty/imgui/")
target_link_libraries(PlazaShippingGame PRIVATE glfw OpenAL freetype TracyClient yaml-cpp mono-2.0-sgen MonoPosixHelper DirectXTex ${PHYSX_LIBS} "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenAL32.lib" Vulkan::Vulkan)

#add_custom_command(TARGET PlazaEngine POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    ${PHYSX_LIBS}
#    ${PHYSX_DLLS}
#    ${OPENAL_DLL}
#    $<TARGET_FILE_DIR:PlazaEngine>)
#
#add_custom_command(TARGET PlazaShippingGame POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different
#    ${PHYSX_LIBS}
#    ${PHYSX_DLLS}
#    ${OPENAL_DLL}
#    $<TARGET_FILE_DIR:PlazaShippingGame>)