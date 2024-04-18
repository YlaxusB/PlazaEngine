#pragma once
#include "Renderer.h"
#include "PostEffects.h"

namespace Plaza {
	class Picking {
	public:
		glm::vec2 mResolution;
		PostEffects* mRenderPickingTexturePostEffects;
		PostEffects* mOutlinePostEffects;
		virtual void Init() = 0;
		virtual void DrawSelectedObjectsUuid() = 0;
		virtual void DrawOutline() = 0;
		virtual uint64_t ReadPickingTexture(glm::vec2 position) = 0;
		virtual uint64_t DrawAndRead(glm::vec2 position) = 0;
		virtual void Terminate() = 0;
	};
}