#ifndef PLAZA_MATERIAL_H
#define PLAZA_MATERIAL_H

#include "Engine/Components/Component.h"
//#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Core/Renderer/Texture.h"
#include "Engine/Core/AssetsManager/Asset.h"

namespace Plaza {
	struct Material : public Asset {
	public:
		unsigned int mIndexHandle = -1;
		Texture* diffuse = new Texture(glm::vec4(1.0f), 1.0f);
		Texture* normal = new Texture(glm::vec4(1.0f), 1.0f);
		Texture* height = new Texture(glm::vec4(1.0f), 1.0f);
		Texture* metalness = new Texture(glm::vec4(1.0f), 0.35f);
		Texture* roughness = new Texture(glm::vec4(1.0f), 1.0f);
		Texture* aoMap = new Texture(glm::vec4(1.0f), 1.0f);
		glm::vec2 flip = glm::vec2(1.0f);

		//Texture* albedo = new Texture("albedo");
		//Texture* specular = new Texture("specular");

		//float shininess = 3.0f;
		//float intensity = 1.0f;
		//float metalnessFloat = 0.35f;
		//float roughnessFloat = 1.0f;

		void LoadTextures(std::string relativePath = "") {
			diffuse->Load(relativePath);
			//albedo->Load(relativePath);
			normal->Load(relativePath);
			//specular->Load(relativePath);
			height->Load(relativePath);
			metalness->Load(relativePath);
			roughness->Load(relativePath);
		}

		Material() {
			this->mAssetUuid = Plaza::UUID::NewUUID();
		}
		~Material() = default;
		// Copy constructor
		/**/
		//Material& operator=(const Material& other) {
		//	//	//diffuse = new Texture();
		//	this->uuid = other.uuid;
		//	filePath = other.filePath;
		//	name = other.name;
		//	diffuse = other.diffuse;
		//	//albedo = other.albedo;
		//	normal = other.normal;
		//	//specular = other.specular;
		//	height = other.height;
		//	if (this != &other) { // self-assignment check
		//		Component::operator=(other); // if Component is a base class, invoke its copy assignment operator
		//
		//		// Copy non-reference members
		//		relativePath = other.relativePath;
		//		filePath = other.filePath;
		//		name = other.name;
		//	}
		//	return *this;
		//}

		bool SameAs(Material& other) {
			return (
				this->diffuse->SameAs(*other.diffuse) &&
				//this->specular->SameAs(*other.specular) &&
				this->normal->SameAs(*other.normal) &&
				this->height->SameAs(*other.height)
				);
		}
		static std::vector<uint64_t> GetMaterialsUuids(std::vector<Material*>& materials) {
			std::vector<uint64_t> materialsUuid = std::vector<uint64_t>();
			for (Material* material : materials) {
				materialsUuid.push_back(material->mAssetUuid);
			}
			return materialsUuid;
		}

		template <class Archive>
		void serialize(Archive& archive) {
			archive(mAssetUuid, mAssetName, diffuse, normal, metalness, roughness, height, aoMap, flip);
		}
	};

}
#endif // PLAZA_MATERIAL_H