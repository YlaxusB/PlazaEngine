#pragma once
#include "Engine/Shaders/Shader.h"
#include "Engine/Components/Core/CoreComponents.h"
#include "Engine/Editor/Editor.h"
#include "Engine/Core/FrameBuffer.h"

namespace Plaza::Editor {
	class Outline {
	public:
		static unsigned int quadVAO, quadVBO;
		static Shader* blurringShader;
		static Shader* combiningShader;

		static FrameBuffer* combiningBuffer;
		static FrameBuffer* blurringBuffer;
		static FrameBuffer* edgeDetectionBuffer;

		static void Init();
		static void RenderOutline();
		static void BlurBuffer();
		static void RenderSelectedObjects(Entity* entity, Shader shader);
		static void CombineOutlineToScene();
		static void CreateScreenQuad();
	};
}