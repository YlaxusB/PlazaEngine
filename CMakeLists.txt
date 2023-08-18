# Set the path to the extracted PhysX SDK
set(PHYSX_SDK_PATH "C:/Users/Giovane/Desktop/Workspace 2023/Physx/physx")

# Add PhysX includes to your project
include_directories(${PHYSX_SDK_PATH}/include)

# Add PhysX libraries to link against
link_directories(${PHYSX_SDK_PATH}/bin/win.x86_64.vc143.mt/checked)