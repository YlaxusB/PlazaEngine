#pragma once
#include "Engine/Shaders/ComputeShader.h"
namespace Plaza {

	class ScreenSpaceReflections {
	public:
		static FrameBuffer* mScreenSpaceReflectionsFbo;
		static Shader* mScreenSpaceReflectionsShader;
		static void Init();
		static void Update();
	};
}