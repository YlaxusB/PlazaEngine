#pragma once
#include <string>
#include <vector>
namespace Plaza {
	class Texture {
	public:
		int id = -1;
		std::string type = "";
		std::string path = "";
		glm::vec4 rgba = glm::vec4(255.0f);

		bool SameAs(Texture& other) {
			return (
				this->path == other.path &&
				this->rgba == other.rgba &&
				this->type == other.type
				);
		}

		void Load(std::string relativePath = "");

		Texture() = default;

		Texture(std::string type) {
			this->type = type;
		}

		// Assignment operator
		Texture& operator=(const Texture& other) {
			if (this != &other) {
				id = other.id;
				type = other.type;
				path = other.path;
				rgba = other.rgba;
			}
			return *this;
		}

		bool IsTextureEmpty() const {
			return rgba == glm::vec4(255.0f) && path.empty();
		}
		~Texture() = default;
	};
}