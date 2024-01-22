#pragma once
#include "Engine/Core/Renderer/Texture.h"

namespace Plaza {
	class OpenGLTexture : public Texture {
	public:
		unsigned int id = -1;
		void Load(std::string relativePath = "") override;
		unsigned int GetTextureID() override;
	};
}