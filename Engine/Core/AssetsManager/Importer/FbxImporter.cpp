#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsImporter.h"
#include "ThirdParty/tinyobjloader/tiny_obj_loader.h"
#include "ThirdParty/glm/gtx/hash.hpp"
#include "Engine/Core/Renderer/Mesh.h"
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Editor/GUI/Utils/Filesystem.h"



#include <d3d11.h>
#include <tchar.h>
#include <stdio.h>
#include <inttypes.h>
#include <vector>

struct Vec3Hash {
	std::size_t operator()(const glm::vec3& v) const {
		return std::hash<glm::vec3>()(v);
	}
};


namespace Plaza {

	static std::string toStringView(ofbx::DataView data) {
		return std::string(
			(const char*)data.begin,
			(const char*)data.end
		);
	}
	Material* AssetsImporter::FbxModelMaterialLoader(const ofbx::Material* ofbxMaterial, const std::string materialFolderPath, std::unordered_map<std::string, uint64_t>& loadedTextures) {
		Material* material = new Material();
		material->name = ofbxMaterial->name;

		const ofbx::Texture* ofbxDiffuse = ofbxMaterial->getTexture(ofbx::Texture::DIFFUSE);
		if (ofbxDiffuse) {
			const std::string diffusePath = materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxDiffuse->getFileName()) }.filename().string();
			if (ofbxDiffuse && loadedTextures.find(diffusePath) == loadedTextures.end())
			{
				material->diffuse = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(diffusePath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(diffusePath, material->diffuse->mAssetUuid);
			}
			else
				material->diffuse = AssetsManager::mTextures.at(loadedTextures.at(diffusePath));
		}

		const ofbx::Texture* ofbxNormal = ofbxMaterial->getTexture(ofbx::Texture::NORMAL);
		if (ofbxNormal) {
			const std::string normalPath = materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxNormal->getFileName()) }.filename().string();
			if (loadedTextures.find(normalPath) == loadedTextures.end())
			{
				material->normal = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(normalPath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(normalPath, material->normal->mAssetUuid);
			}
			else
				material->normal = AssetsManager::mTextures.at(loadedTextures.at(normalPath));
		}

		const ofbx::Texture* ofbxSpecular = ofbxMaterial->getTexture(ofbx::Texture::SPECULAR);
		if (ofbxSpecular) {
			const std::string specularPath = materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxSpecular->getFileName()) }.filename().string();
			if (ofbxSpecular && loadedTextures.find(specularPath) == loadedTextures.end())
			{
				material->roughness = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(specularPath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(specularPath, material->roughness->mAssetUuid);
			}
			else
				material->roughness = AssetsManager::mTextures.at(loadedTextures.at(specularPath));
		}

		const ofbx::Texture* ofbxReflection = ofbxMaterial->getTexture(ofbx::Texture::REFLECTION);
		if (ofbxReflection) {
			const std::string reflectionPath = materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxReflection->getFileName()) }.filename().string();
			if (ofbxReflection && loadedTextures.find(reflectionPath) == loadedTextures.end())
			{
				material->metalness = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(reflectionPath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(reflectionPath, material->metalness->mAssetUuid);
			}
			else
				material->metalness = AssetsManager::mTextures.at(loadedTextures.at(reflectionPath));
		}
		return material;
	}

	Entity* AssetsImporter::ImportFBX(AssetImported asset, std::filesystem::path outPath) {
		FILE* fileOpen = fopen(asset.mPath.c_str(), "rb");

		if (!fileOpen) return nullptr;

		fseek(fileOpen, 0, SEEK_END);
		long fileSize = ftell(fileOpen);
		fseek(fileOpen, 0, SEEK_SET);
		auto* content = new ofbx::u8[fileSize];
		fread(content, 1, fileSize, fileOpen);

		ofbx::LoadFlags flags =
			//		ofbx::LoadFlags::IGNORE_MODELS |
			ofbx::LoadFlags::IGNORE_BLEND_SHAPES |
			ofbx::LoadFlags::IGNORE_CAMERAS |
			ofbx::LoadFlags::IGNORE_LIGHTS |
			//		ofbx::LoadFlags::IGNORE_TEXTURES |
			ofbx::LoadFlags::IGNORE_SKIN |
			ofbx::LoadFlags::IGNORE_BONES |
			ofbx::LoadFlags::IGNORE_PIVOTS |
			//		ofbx::LoadFlags::IGNORE_MATERIALS |
			ofbx::LoadFlags::IGNORE_POSES |
			ofbx::LoadFlags::IGNORE_VIDEOS |
			ofbx::LoadFlags::IGNORE_LIMBS |
			//		ofbx::LoadFlags::IGNORE_MESHES |
			ofbx::LoadFlags::IGNORE_ANIMATIONS;

		ofbx::IScene* loadedScene = ofbx::load((ofbx::u8*)content, fileSize, (ofbx::u16)flags);
		delete[] content;
		fclose(fileOpen);

		uint64_t verticesCount = 0;

		int meshCount = loadedScene->getMeshCount();

		// output unindexed geometry
		Entity* mainEntity = nullptr;
		std::unordered_map<ofbx::u64, uint64_t> meshIndexEntityMap = std::unordered_map<ofbx::u64, uint64_t>();
		std::unordered_map<std::string, uint64_t> loadedTextures = std::unordered_map<std::string, uint64_t>();
		std::unordered_map<std::filesystem::path, uint64_t> loadedMaterials = std::unordered_map<std::filesystem::path, uint64_t>();


		for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
			const ofbx::Mesh& mesh = *loadedScene->getMesh(meshIndex);
			const ofbx::Material* ofbxMaterial = mesh.getMaterial(0);
			Material* material;

			material = AssetsImporter::FbxModelMaterialLoader(ofbxMaterial, std::filesystem::path{ asset.mPath }.parent_path().string(), loadedTextures);

			std::filesystem::path materialOutPath = Editor::Gui::FileExplorer::currentDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(Editor::Gui::FileExplorer::currentDirectory + "\\" + material->name) + Standards::materialExtName;

			if (loadedMaterials.find(materialOutPath) == loadedMaterials.end()) {
				loadedMaterials.emplace(materialOutPath, material->uuid);
				AssetsSerializer::SerializeMaterial(material, materialOutPath);
				Application->activeScene->AddMaterial(material);
			}
			else
				material = Application->activeScene->GetMaterial(loadedMaterials.find(materialOutPath)->second);

			Entity* entity;
			if (!mainEntity)
			{
				entity = new Entity(mesh.name, Application->activeScene->mainSceneEntity);
				mainEntity = entity;
			}
			else
				entity = new Entity(mesh.name, mainEntity, true);

			meshIndexEntityMap.emplace(mesh.id, entity->uuid);
			ofbx::Object* parent = mesh.getParent();
			if (parent) {
				auto parentIt = meshIndexEntityMap.find(parent->id);
				if (parentIt != meshIndexEntityMap.end()) {
					//entity->ChangeParent(Application->activeScene->GetEntity(entity->GetParent().uuid), Application->activeScene->GetEntity(parentIt->second));
				}
			}


			const ofbx::GeometryData& geom = mesh.getGeometryData();
			const ofbx::Vec3Attributes positions = geom.getPositions();
			const ofbx::Vec3Attributes normals = geom.getNormals();
			const ofbx::Vec2Attributes uvs = geom.getUVs();

			Mesh* finalMesh = new Mesh();

			/* TODO: FIX UNIQUE VERTICES, SO THEY ALSO DIFFER FROM NORMAL AND UV */
			std::unordered_map<glm::vec3, uint32_t> uniqueVertices = std::unordered_map<glm::vec3, uint32_t>();
			//int* tri_indices = new int();
			int indicesOffset = 0;
			for (int partitionIdx = 0; partitionIdx < geom.getPartitionCount(); ++partitionIdx) {
				if (partitionIdx > 0)
					std::cout << partitionIdx << "\n";
				const ofbx::GeometryPartition& partition = geom.getPartition(partitionIdx);
				std::vector<int> tri_indices(partition.max_polygon_triangles * 3);


				for (int polygon_idx = 0; polygon_idx < partition.polygon_count; ++polygon_idx) {
					const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygon_idx];
					uint64_t triangleCount = ofbx::triangulate(geom, polygon, tri_indices.data());

					for (int i = 0; i < triangleCount; ++i) {
						//for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
						ofbx::Vec3 v = positions.get(tri_indices[i]);
						glm::vec3 vertex = glm::vec3(v.x, v.y, v.z);
						bool isVertexUnique = uniqueVertices.count(vertex) == 0;
						if (isVertexUnique) {
							uniqueVertices[vertex] = static_cast<uint32_t>(finalMesh->vertices.size());
							finalMesh->vertices.push_back(glm::vec3(v.x, v.y, v.z));
							ofbx::Vec3 n = normals.get(tri_indices[i]);
							finalMesh->normals.push_back(glm::vec3(n.x, n.y, n.z));
							ofbx::Vec2 u = uvs.get(tri_indices[i]);
							finalMesh->uvs.push_back(glm::vec2(u.x, u.y));
						}
						finalMesh->indices.push_back(uniqueVertices[vertex]);
					}
				}
				indicesOffset += positions.count;
			}

			MeshRenderer* meshRenderer = new MeshRenderer(finalMesh, Application->activeScene->DefaultMaterial());
			meshRenderer->material = material;//AssetsLoader::LoadMaterial(AssetsManager::GetAsset(std::filesystem::path{ Editor::Gui::FileExplorer::currentDirectory + "\\" + material->name + Standards::materialExtName}.string()));
			entity->AddComponent<MeshRenderer>(meshRenderer);
			verticesCount += finalMesh->vertices.size();
		}

		return Application->activeScene->GetEntity(mainEntity->uuid);
	}
}
