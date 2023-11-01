#include "Engine/Core/PreCompiledHeaders.h"
#include "Texture.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
namespace Plaza {
	void Texture::Load(std::string relativePath) {
		if (!path.empty()) {
			if (!relativePath.empty() && path.starts_with(".\\")) {
				this->id = ModelLoader::TextureFromFile(relativePath + "\\" + path);
			}
			else {
				this->id = ModelLoader::TextureFromFile(path);
			}
			this->rgba = glm::vec4(INFINITY);
		}
	}
}