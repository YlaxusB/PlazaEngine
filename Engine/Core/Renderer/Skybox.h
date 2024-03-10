#pragma once
#include "Renderer.h"
#include "PostEffects.h"

namespace Plaza {
	class Skybox {
	public:
		glm::vec2 mResolution;
		PostEffects* mSkyboxPostEffect;
		virtual void Init() = 0;
		virtual void DrawSkybox() = 0;
		virtual void Termiante() = 0;
	};
}