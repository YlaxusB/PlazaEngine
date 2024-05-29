#pragma once
#include <Engine/Core/Renderer/Lighting.h>
#include "VulkanPostEffects.h"

namespace Plaza {
	class VulkanLighting : public Lighting {
	public:
		void Init() override;
		void UpdateTiles() override;
		void Terminate() override;
	}
}