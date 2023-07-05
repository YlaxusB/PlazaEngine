#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Core/Component.h"
#include "Engine/Components/Core/Mesh.h"
namespace Engine {
	struct Texture;
	struct Material : public Component {
	public:
		Texture* diffuse;
		Texture* albedo;
		Texture* normal;
		Texture* specular;
		Texture* height;
		float shininess = 64.0f;

		Material() = default;
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