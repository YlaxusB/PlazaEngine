#include "Engine/Core/PreCompiledHeaders.h"
#include "OpenGLTexture.h"
#include "Renderer.h"

namespace Plaza {
	void OpenGLTexture::Load(std::string relativePath) {
		if (!mAssetPath.empty()) {
			if (!relativePath.empty() && mAssetPath.string().starts_with(".\\")) {
				this->id = Application::Get()->mRenderer->LoadTexture(relativePath + "\\" + mAssetPath.string())->GetTextureID(); //this->id = ModelLoader::TextureFromFile(relativePath + "\\" + path);
			}
			else if (!relativePath.empty()) {
				this->id = Application::Get()->mRenderer->LoadTexture(relativePath + "\\" + mAssetPath.string())->GetTextureID(); //this->id = ModelLoader::TextureFromFile(relativePath + "\\" + path);
			}
			else
				this->id = Application::Get()->mRenderer->LoadTexture(mAssetPath.string())->GetTextureID();
				//this->id = ModelLoader::TextureFromFile(path);
			this->rgba = glm::vec4(INFINITY);
		}
	}

	unsigned int OpenGLTexture::GetTextureID() {
		return this->id;
	}

	ImTextureID OpenGLTexture::GetImGuiTextureID() {
		return (ImTextureID)this->id;
	}
} 