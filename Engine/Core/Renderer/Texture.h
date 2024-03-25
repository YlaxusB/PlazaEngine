#pragma once
#include <string>
#include <vector>
namespace Plaza {
	class Texture {
	public:
		uint64_t mAssetUuid;
		std::string type = "";
		std::string path = "";
		glm::vec4 rgba = glm::vec4(255.0f);
		int mIndexHandle = -1;

		bool SameAs(Texture& other) {
			return (
				this->path == other.path &&
				this->rgba == other.rgba &&
				this->type == other.type
				);
		}

		Texture() = default;

		virtual void Load(std::string relativePath = "") {}
		virtual VkDescriptorSet GetDescriptorSet() {
			return VkDescriptorSet();
		}

		Texture(std::string type) {
			this->type = type;
		}

		// Assignment operator
		Texture& operator=(const Texture& other) {
			if (this != &other) {
				type = other.type;
				path = other.path;
				rgba = other.rgba;
			}
			return *this;
		}

		bool IsTextureEmpty() const {
			return (rgba == glm::vec4(255.0f) && path.empty()) || (rgba == glm::vec4(255.0f) && mIndexHandle < 0);
		}

		virtual unsigned int GetTextureID() {
			return 0;
		}

		virtual ImTextureID GetImGuiTextureID() {
			return 0;
		}

		~Texture() = default;
	};
}

/*
#include "Engine/Core/PreCompiledHeaders.h"
#include "Texture.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
namespace Plaza {
	void Texture::Load(std::string relativePath) {
		if (!path.empty()) {
			if (!relativePath.empty() && path.starts_with(".\\")) {
				this->id = ModelLoader::TextureFromFile(relativePath + "\\" + path);
			}
			else if (!relativePath.empty()) {
				this->id = ModelLoader::TextureFromFile(relativePath + "\\" + path);
			}
			else
				this->id = ModelLoader::TextureFromFile(path);
			this->rgba = glm::vec4(INFINITY);
		}
	}
}
*/