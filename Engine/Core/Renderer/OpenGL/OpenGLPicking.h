#pragma once
#include <Engine/Core/Renderer/Picking.h>

namespace Plaza {
	class OpenGLPicking : public Picking {
	public:
		struct PushConstants {
			glm::mat4 projection;
			glm::mat4 view;
			uint64_t uuid;
		} pushData;
		void Init() override;
		void DrawSelectedObjectsUuid() override;
		void DrawOutline() override;
		uint64_t ReadPickingTexture(glm::vec2 position) override;
		uint64_t DrawAndRead(glm::vec2 position) override;
		void Terminate() override;

	private:

	};
}