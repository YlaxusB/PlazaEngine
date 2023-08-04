#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <Engine/Vendor/glad/glad.h>
#include <Engine/Vendor/glm/glm.hpp>
#include <Engine/Vendor/glm/gtc/matrix_transform.hpp>
#include <Engine/Vendor/glm/gtc/type_ptr.hpp>
#include <Engine/Vendor/glm/gtx/matrix_decompose.hpp>
#include <Engine/Vendor/glm/trigonometric.hpp>
#include <Engine/Vendor/glm/gtx/euler_angles.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <Engine/Vendor/glm/gtx/quaternion.hpp>
#include <Engine/Vendor/stb/stb_image.h>
#include <Engine/Vendor/imgui/imgui_internal.h>
#include <Engine/Vendor/imgui/imgui.h>
#include <Engine/Vendor/imgui/imgui_impl_glfw.h>
#include <Engine/Vendor/imgui/imgui_impl_opengl3.h>
#include <Engine/Vendor/imgui/ImGuizmo.h>
#include "Engine/Vendor/yaml/include/yaml-cpp/yaml.h"
#include "Engine/Vendor/Json/json.hpp" // Include the nlohmann/json library
#include "Engine/Utils/yamlUtils.h"

#include <iostream>
#include <random>
#include <unordered_map>
#include <filesystem>
#include "Engine/Vendor/filesystem/filesys.h"
#include <list>
#include <string>
#include <chrono>
#include <variant>

#include "Engine/Application/Application.h"
#include "Engine/Application/EntryPoint.h"
#include "Engine/Core/Standards.h"
#include "Engine/Core/UUID.h"