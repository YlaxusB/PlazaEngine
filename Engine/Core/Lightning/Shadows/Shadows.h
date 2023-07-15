#pragma once
#include "Engine/GUI/guiMain.h"
#include "Engine/Editor/Editor.h"

namespace Engine {
    class ShadowsClass {
    public:
        unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
        const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
        glm::vec3 lightDistance = glm::vec3(100.0f, 400.0f, 0.0f);
        glm::vec3 lightPos;
        float near_plane = -500.0f, far_plane = 500.0f;
        glm::mat4 lightProjection = glm::ortho(-100.0f, 500.0f, -100.0f, 500.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        unsigned int shadowsFBO, shadowsDepthMap, matricesUBO = 0;
        bool showDepth = false;
        void Init();
        void GenerateDepthMap();
        void RenderScene(Shader& shader);
        void Terminate();
    };
}