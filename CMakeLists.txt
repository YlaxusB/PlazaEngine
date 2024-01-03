cmake_minimum_required(VERSION 3.13)

add_subdirectory("ThirdParty/Tracy")

project("PlazaEngine")
set(CMAKE_CXX_STANDARD 20)
add_executable("PlazaEngine" "Engine/Application/EntryPoint.cpp")

include_directories("./")
include_directories("ThirdParty/")
include_directories("ThirdParty/AL")
include_directories("ThirdParty/yaml/include")

# Set the path to the extracted PhysX SDK
set(PHYSX_SDK_PATH "PhysX/physx")

# Add PhysX includes to your project
include_directories(${PHYSX_SDK_PATH}/include)

# Add PhysX libraries to link against
link_directories(${PHYSX_SDK_PATH}/bin/win.x86_64.vc143.mt/checked)

target_include_directories(PlazaEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")
target_link_libraries(PlazaEngine PRIVATE Tracy)