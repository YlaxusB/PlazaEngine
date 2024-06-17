#pragma once
#include "Engine/Components/Component.h"
//#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Core/Renderer/Texture.h"
namespace Plaza {
	struct Material : public Component {
	public:
		unsigned int mIndexHandle = -1;
		uint64_t mAssetUuid = 0;
		std::string relativePath;
		std::string filePath;
		std::string name = "";
		Texture* diffuse = new Texture("diffuse");
		Texture* albedo = new Texture("albedo");
		Texture* normal = new Texture("normal");
		Texture* specular = new Texture("specular");
		Texture* height = new Texture("height");
		Texture* metalness = new Texture("metalness");
		Texture* roughness = new Texture("roughness");
		Texture* aoMap = new Texture("aoMap");
		float shininess = 3.0f;
		float intensity = 1.0f;

		float metalnessFloat = 0.0f;
		float roughnessFloat = 0.5f;

		void LoadTextures(std::string relativePath = "") {
			diffuse->Load(relativePath);
			albedo->Load(relativePath);
			normal->Load(relativePath);
			specular->Load(relativePath);
			height->Load(relativePath);
			metalness->Load(relativePath);
			roughness->Load(relativePath);
		}

		Material() {
			this->uuid = Plaza::UUID::NewUUID();
			this->mAssetUuid = this->uuid;
		}
		~Material() = default;
		// Copy constructor
		/**/
		Material& operator=(const Material& other) {
			//	//diffuse = new Texture();
			this->uuid = other.uuid;
			filePath = other.filePath;
			name = other.name;
			shininess = other.shininess;
			metalnessFloat = other.metalnessFloat;
			roughnessFloat = other.roughnessFloat;
			diffuse = other.diffuse;
			albedo = other.albedo;
			normal = other.normal;
			specular = other.specular;
			height = other.height;
			if (this != &other) { // self-assignment check
				Component::operator=(other); // if Component is a base class, invoke its copy assignment operator

				// Copy non-reference members
				relativePath = other.relativePath;
				filePath = other.filePath;
				name = other.name;
				shininess = other.shininess;
				intensity = other.intensity;
			}
			return *this;
		}
		//Material(const Material& other) {
		//	//diffuse = new Texture();
		//	//this->uuid = other.uuid;
		//	//filePath = other.filePath;
		//	//name = other.name;
		//	//shininess = other.shininess;
		//	//diffuse = other.diffuse;
		//	//albedo = other.albedo;
		//	//normal = other.normal;
		//	//specular = other.specular;
		//	//height = other.height;
		//}

		bool SameAs(Material& other) {
			return (
				this->diffuse->SameAs(*other.diffuse) &&
				this->specular->SameAs(*other.specular) &&
				this->normal->SameAs(*other.normal) &&
				this->height->SameAs(*other.height) &&
				this->shininess == other.shininess
				);
		}
		static std::vector<uint64_t> GetMaterialsUuids(std::vector<Material*>& materials) {
			std::vector<uint64_t> materialsUuid = std::vector<uint64_t>();
			for (Material* material : materials) {
				materialsUuid.push_back(material->uuid);
			}
			return materialsUuid;
		}
	};

}