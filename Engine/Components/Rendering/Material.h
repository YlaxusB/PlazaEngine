#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Components/Rendering/Texture.h"
namespace Plaza {
	struct Material : public Component {
	public:
		std::string relativePath;
		std::string filePath;
		std::string name;
		Texture diffuse = Texture("diffuse");
		Texture albedo = Texture("albedo");
		Texture normal = Texture("normal");
		Texture specular = Texture("specular");
		Texture height = Texture("height");
		float shininess = 3.0f;

		void LoadTextures(std::string relativePath = "") {
			diffuse.Load(relativePath);
			albedo.Load(relativePath);
			normal.Load(relativePath);
			specular.Load(relativePath);
			height.Load(relativePath);
		}

		Material() {
			this->uuid = Plaza::UUID::NewUUID();
		}
		~Material() = default;
		// Copy constructor
		/**/
		Material(const Material& other) {
			//diffuse = new Texture();
			this->uuid = other.uuid;
			filePath = other.filePath;
			name = other.name;
			shininess = other.shininess;
			diffuse = other.diffuse;
			albedo = other.albedo;
			normal = other.normal;
			specular = other.specular;
			height = other.height;
		}

		bool SameAs(Material& other) {
			return (
				this->diffuse.SameAs(other.diffuse) &&
				this->specular.SameAs(other.specular) &&
				this->normal.SameAs(other.normal) &&
				this->height.SameAs(other.height) &&
				this->shininess == other.shininess
				);
		}
	};
}