#pragma once
#include <string>
#include <vector>
namespace Engine {
	struct Texture {
	public:
		int id = -1;
		std::string type = "";
		std::string path = "";
		glm::vec4 rgba = glm::vec4(INFINITY);

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
			return rgba == glm::vec4(INFINITY) && path.empty();
		}
		~Texture() = default;
	};
}