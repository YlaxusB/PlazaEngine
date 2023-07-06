#pragma once
#include "Engine/Vendor/GLFW/glfw3.h"
#include "Engine/Application/Application.h"

#include "Engine/GUI/guiMain.h"
#include "Engine/Editor/Editor.h"

namespace Engine {
    class ShadowsClass {
    public:
        const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
        const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
        glm::vec3 lightDistance = glm::vec3(40.0f, 40.0f, 0.0f);
        glm::vec3 lightPos;
        float near_plane = 1.0f, far_plane = 57.5f;
        glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
        unsigned int shadowsFBO, shadowsDepthMap, matricesUBO = 0;
        bool showDepth = false;
        void Init();
        void GenerateDepthMap();
        void RenderScene(Shader& shader);
        void Terminate();
    };
}