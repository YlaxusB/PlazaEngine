#pragma once
#include "Engine/Shaders/Shader.h"
#include "Engine/Components/Core/CoreComponents.h"
#include "Engine/Editor/Editor.h"

namespace Engine::Editor {
	class Outline {
	public:
		static unsigned int quadVAO, quadVBO;
		static Shader* blurringShader;
		static Shader* combiningShader;

		static void RenderOutline();
		static void BlurBuffer();
		static void RenderSelectedObjects(GameObject* gameObject, Shader shader);
		static void CombineOutlineToScene();
		static void CreateScreenQuad();
	};
}