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
        glm::vec3 lightDistance = glm::vec3(0.0f, 4, 0);
        glm::vec3 lightPos;
        unsigned int shadowsFBO, shadowsDepthMap, matricesUBO = 0;
        void Init();
        void GenerateDepthMap();
        void RenderScene(Shader& shader);
        void Terminate();
    };
}