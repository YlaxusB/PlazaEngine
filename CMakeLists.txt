cmake_minimum_required(VERSION 3.24)

if(POLICY CMP0091) # Force thidparty libraries to use static MSVC
    cmake_policy(SET CMP0091 NEW)
endif()

project(PlazaEngineProject)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_COMPILER g++)

#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:MultiThreaded>$<$<CONFIG:GameRelease>:MultiThreaded>")

if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "")
    add_compile_options(/MP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX- /W4 /O2")

endif()

set(BUILD_STATIC_LIB ON)

set(TARGET_BUILD_PLATFORM "windows")
set(PLATFORM "Windows")

include(FindVulkan)
if(NOT VULKAN_FOUND)
    message(FATAL_ERROR "Vulkan SDK not installed.")
endif()
find_package(Vulkan REQUIRED)

set(PLATFORM_CMAKELISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

message(WARNING "${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    add_compile_definitions(_DEBUG)
    add_definitions(-D_DEBUG)
    add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
    option(PHYSX_BUILD_TYPE "debug")
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Ob0 /Od /RTC1")
    option(MSVC_CRT_STATIC   "Link primesieve.lib with /MT instead of the default /MD" OFF)
    #set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:>")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    add_compile_definitions(NDEBUG)
    add_definitions(-DNDEBUG)
    option(PHYSX_BUILD_TYPE "release")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
    option(MSVC_CRT_STATIC   "Link primesieve.lib with /MT instead of the default /MD" ON)
    #set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()
#add_compile_definitions(<$<CONFIG:Debug>:_DEBUG><$<CONFIG:Debug>:NDEBUG>)
add_definitions(-DLIBTYPE=STATIC)
add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)
#set(CMAKE_BUILD_TYPE Debug)

set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(GLM_FORCE_DEPTH_ZERO_TO_ONE ON)

set(LIBTYPE STATIC)
add_subdirectory("ThirdParty/AL/")
add_subdirectory("ThirdParty/DirectXTex/")
add_subdirectory("ThirdParty/freetype/")
add_subdirectory("ThirdParty/GLFW/")
#add_subdirectory("ThirdParty/spdlog/")
add_subdirectory("ThirdParty/Tracy/")

set(PLAZA_ITERATOR_LEVEL 2)

set(BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/out/build/x64-${CMAKE_BUILD_TYPE}")
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

#link_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/lib/${CMAKE_BUILD_TYPE}")

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

add_executable(PlazaEngine "${IMGUI_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Application/EntryPoint.cpp" "${MY_SOURCES}" "${MY_SOURCES2}")

add_executable(PlazaShippingGame "${IMGUI_SOURCES}" "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Application/EntryPoint.cpp" "${MY_SOURCES}" "${MY_SOURCES2}")

set_target_properties(PlazaEngine PROPERTIES
    VS_GLOBAL_MONO_USE_STATIC_C_RUNTIME "true"
    VS_GLOBAL_MONO_USE_STATIC_LIBMONO "true"
)

set_target_properties(PlazaShippingGame PROPERTIES
    VS_GLOBAL_MONO_USE_STATIC_C_RUNTIME "true"
    VS_GLOBAL_MONO_USE_STATIC_LIBMONO "true"
)

target_compile_definitions(PlazaEngine PUBLIC  PX_PHYSX_STATIC_LIB)
target_compile_definitions(PlazaEngine PUBLIC  EDITOR_MODE)
target_compile_options(PlazaEngine PUBLIC  /WX-)

target_compile_definitions(PlazaShippingGame PUBLIC  GAME_MODE)
target_compile_definitions(PlazaShippingGame PUBLIC  PX_PHYSX_STATIC_LIB)
target_compile_options(PlazaShippingGame PUBLIC  /WX-)

target_precompile_headers(PlazaEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core/PreCompiledHeaders.h")
target_precompile_headers(PlazaShippingGame PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core/PreCompiledHeaders.h")

if(MSVC)
  target_compile_options(PlazaEngine PUBLIC "/ZI")
  target_link_options(PlazaEngine PUBLIC "/INCREMENTAL")

  target_compile_options(PlazaShippingGame PUBLIC "/ZI")
  target_link_options(PlazaShippingGame PUBLIC "/INCREMENTAL")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /C2220")

include_directories("./")
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
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/spdlog/include/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tinyobjloader/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/ufbx/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/uuid_v4/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vswhere/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/vulkan/vulkan/")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/KHR/")

string(TOLOWER "${CMAKE_BUILD_TYPE}" CMAKE_BUILD_TYPE_LOWER)
set(PHYSX_LIBS
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/${CMAKE_BUILD_TYPE_LOWER}/PhysX_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/${CMAKE_BUILD_TYPE_LOWER}/PhysXExtensions_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/${CMAKE_BUILD_TYPE_LOWER}/PhysXCooking_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/${CMAKE_BUILD_TYPE_LOWER}/PhysXCommon_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/${CMAKE_BUILD_TYPE_LOWER}/PhysXFoundation_static.lib"
    "${PX_OUTPUT_LIB_DIR}/bin/win.x86_64.vc143.mt/${CMAKE_BUILD_TYPE_LOWER}/PhysXPvdSDK_static.lib"
)

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

target_include_directories(PlazaEngine PUBLIC "lib/")
target_include_directories(PlazaShippingGame PUBLIC "lib/")

add_dependencies(PlazaEngine PhysX)
add_dependencies(PlazaShippingGame PhysX)

set(MONO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/mono/lib/${CMAKE_BUILD_TYPE}")

#add_library(Mono STATIC IMPORTED)
#set_target_properties(Mono PROPERTIES
#    IMPORTED_LOCATION "${MONO_LIB}" 
#)

target_include_directories(PlazaEngine PUBLIC "ThirdParty/imgui/")
target_link_libraries(PlazaEngine PUBLIC  glfw OpenAL freetype TracyClient "${MONO_PATH}/mono-2.0-sgen.lib" "${MONO_PATH}/libmono-static-sgen.lib"  DirectXTex ${PHYSX_LIBS} "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenAL32.lib" Vulkan::Vulkan)

target_include_directories(PlazaShippingGame PUBLIC "ThirdParty/imgui/")
target_link_libraries(PlazaShippingGame PUBLIC  glfw OpenAL freetype TracyClient "${MONO_PATH}/mono-2.0-sgen.lib" "${MONO_PATH}/libmono-static-sgen.lib" DirectXTex ${PHYSX_LIBS} "${CMAKE_CURRENT_SOURCE_DIR}/lib/OpenAL32.lib" Vulkan::Vulkan)