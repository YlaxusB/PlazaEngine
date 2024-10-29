cmake_minimum_required(VERSION 3.25)

if(POLICY CMP0091) # Force thidparty libraries to use static MSVC
    cmake_policy(SET CMP0091 NEW)
endif()

project(GameProject)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    #set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "")
    add_compile_options(/MP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX- /W4 /O2")
    add_compile_options(/bigobj)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "Embedded")
endif()

#if(POLICY CMP0141)
#  cmake_policy(SET CMP0141 NEW)
#  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
#endif()

set(BUILD_STATIC_LIB ON)
set(TARGET_BUILD_PLATFORM "windows")
set(PLATFORM "Windows")
add_compile_definitions(_DEBUG)
add_definitions(-D_DEBUG)
add_definitions(-D_ITERATOR_DEBUG_LEVEL=2)
add_definitions(-DLIBTYPE=STATIC)
add_compile_definitions(IMGUI_DEFINE_MATH_OPERATORS)
option(MSVC_CRT_STATIC   "Link primesieve.lib with /MT instead of the default /MD" OFF)
set(PLATFORM_CMAKELISTS "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "/Ob0 /Od /RTC1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4244")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /C2220")
set(PLAZA_ITERATOR_LEVEL 2)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(ENGINE_SOURCE_DIR "C:/Users/Giovane/Desktop/Workspace/PlazaEngine")


file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Assets/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/Scripts/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Scripts/*.h")
add_library(GameLib SHARED ${GAME_SOURCES})

target_compile_definitions(GameLib PUBLIC  PX_PHYSX_STATIC_LIB)
target_compile_definitions(GameLib PUBLIC  EDITOR_MODE)
target_compile_definitions(GameLib PUBLIC  COMPILING_GAME_DLL)
  target_link_options(GameLib PUBLIC "/INCREMENTAL /PDBALTPATH:$<TARGET_PDB_FILE_NAME:GameLib>")

target_include_directories(GameLib PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/"
    "${ENGINE_SOURCE_DIR}/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/AL/include/AL"
    "${ENGINE_SOURCE_DIR}/ThirdParty/cereal/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/DirectXTex/DirectXTex/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/filesystem/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/FileWathcer/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/freetype/include/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/glad/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/GLFW/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/glm/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/Json/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/KHR/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/minimp3/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/mono/include/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/PhysX/physx/include/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/premake/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/stb/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/spdlog/include/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/tinyobjloader/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/ufbx/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/uuid_v4/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/vswhere/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/vulkan/"
    "${ENGINE_SOURCE_DIR}/ThirdParty/vulkan/vulkan/"
)

set_property(TARGET GameLib PROPERTY CXX_STANDARD 20)

target_link_libraries(GameLib PUBLIC PlazaEngineCore)

#set_target_properties(GameLib PROPERTIES
#    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
#)

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")
add_custom_command(TARGET GameLib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTPUT_DIRECTORY}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:GameLib>"
    "$<TARGET_PDB_FILE:GameLib>"
    "${OUTPUT_DIRECTORY}"
)

#add_custom_command(TARGET GameLib POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E remove
#    "$<TARGET_PDB_FILE:GameLib>"
#)

add_custom_target(CopyGameLib ALL DEPENDS GameLib)
add_custom_target(DeleteGamePDB ALL DEPENDS GameLib)