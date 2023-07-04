#pragma once
#include <GLFW/glfw3.h>
#include "Engine/Application/Application.h"

#include "Engine/GUI/guiMain.h"
#include "Engine/Editor/Editor.h"

namespace Engine {
    class ShadowsClass {
    public:
        const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

        unsigned int shadowsFBO, shadowsDepthMap = 0;
        void Init();
        void GenerateDepthMap();
        void RenderScene(Shader& shader);
        void Terminate();
    };
}