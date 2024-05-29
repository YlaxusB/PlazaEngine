#pragma once
#include <Engine/Core/Renderer/Lighting.h>

namespace Plaza {
	class OpenGLLighting : public Lighting {
	public:
		void Init() override;
		void UpdateTiles() override;
		void Terminate() override;
	}
}