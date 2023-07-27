#pragma once
#include "Editor/GUI/guiMain.h"
#include "Engine/Editor/Editor.h"

namespace Engine {
    class ShadowsClass {
    public:
        unsigned int depthMapResolution = 4096;
        std::vector<float> shadowCascadeLevels;
        unsigned int matricesUBO, maximumCascades = 32;
        unsigned int shadowWidth = 2048, shadowHeight = 2048;
        const glm::vec3 lightDir = glm::normalize(glm::vec3(20.0f, 50, 20.0f));
        glm::vec3 lightDistance = glm::vec3(100.0f, 400.0f, 0.0f);
        glm::vec3 lightPos;
        float near_plane = -500.0f, far_plane = 500.0f;
        glm::mat4 lightProjection = glm::ortho(-100.0f, 500.0f, -100.0f, 500.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        unsigned int shadowsFBO, shadowsDepthMap;
        bool showDepth = false;
        void Init();
        void GenerateDepthMap();
        void RenderScene(Shader& shader);
        void Terminate();
        std::vector<glm::mat4> GetLightSpaceMatrices();
    };
}