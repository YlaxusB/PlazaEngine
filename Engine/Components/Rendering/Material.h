#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Components/Rendering/Texture.h"
namespace Engine {
	struct Material : public Component {
	public:
		Texture diffuse;
		Texture albedo;
		Texture normal;
		Texture specular;
		Texture height;
		float shininess = 64.0f;

		Material() = default;
		~Material() = default;
		// Copy constructor
		/**/
		Material(const Material& other) {
			//diffuse = new Texture();
			diffuse = other.diffuse;
			albedo = other.albedo;
			normal = other.normal;
			specular = other.specular;
			height = other.height;
		}
	};
}