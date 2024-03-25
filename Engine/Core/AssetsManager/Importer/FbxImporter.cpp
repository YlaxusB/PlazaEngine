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
	Material* AssetsImporter::FbxModelMaterialLoader(const ofbx::Material* ofbxMaterial, const std::string materialFolderPath) {
		Material* material = new Material();
		material->name = ofbxMaterial->name;

		const ofbx::Texture* ofbxDiffuse = ofbxMaterial->getTexture(ofbx::Texture::DIFFUSE);
		if (ofbxDiffuse)
			material->diffuse = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxDiffuse->getFileName()) }.filename().string()));

		const ofbx::Texture* ofbxNormal = ofbxMaterial->getTexture(ofbx::Texture::NORMAL);
		if (ofbxNormal)
			material->normal = Application->mRenderer->LoadTexture(AssetsManager::GetAssetOrImport(materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxNormal->getFileName()) }.filename().string())->mPath.string());

		const ofbx::Texture* ofbxSpecular = ofbxMaterial->getTexture(ofbx::Texture::SPECULAR);
		if (ofbxSpecular)
			material->roughness = Application->mRenderer->LoadTexture(AssetsManager::GetAssetOrImport(materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxSpecular->getFileName()) }.filename().string())->mPath.string());

		const ofbx::Texture* ofbxReflection = ofbxMaterial->getTexture(ofbx::Texture::REFLECTION);
		if (ofbxReflection)
			material->diffuse = Application->mRenderer->LoadTexture(AssetsManager::GetAssetOrImport(materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxReflection->getFileName()) }.filename().string())->mPath.string());

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
		for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
			const ofbx::Mesh& mesh = *loadedScene->getMesh(meshIndex);
			const ofbx::Material* ofbxMaterial = mesh.getMaterial(0);
			Material* material = AssetsImporter::FbxModelMaterialLoader(ofbxMaterial, std::filesystem::path{ asset.mPath }.parent_path().string());
			AssetsSerializer::SerializeMaterial(material, Editor::Gui::FileExplorer::currentDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(Editor::Gui::FileExplorer::currentDirectory + "\\" + material->name) + Standards::materialExtName);

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
			std::vector<int> tri_indices = std::vector<int>();
			int indicesOffset = 0;
			for (int partitionIdx = 0; partitionIdx < geom.getPartitionCount(); ++partitionIdx) {
				if (partitionIdx > 0)
					std::cout << partitionIdx << "\n";
				const ofbx::GeometryPartition& partition = geom.getPartition(partitionIdx);
				tri_indices.resize(tri_indices.size() + partition.max_polygon_triangles * 3);

				for (int polygon_idx = 0; polygon_idx < partition.triangles_count; ++polygon_idx) {
					const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygon_idx];
					for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
						ofbx::Vec3 v = positions.get(i);
						glm::vec3 vertex = glm::vec3(v.x, v.y, v.z);
						bool isVertexUnique = uniqueVertices.count(vertex) == 0;
						if (isVertexUnique) {
							uniqueVertices[vertex] = static_cast<uint32_t>(finalMesh->vertices.size());
							finalMesh->vertices.push_back(glm::vec3(v.x, v.y, v.z));
							ofbx::Vec3 n = normals.get(i);
							finalMesh->normals.push_back(glm::vec3(n.x, n.y, n.z));
							ofbx::Vec2 u = uvs.get(i);
							finalMesh->uvs.push_back(glm::vec2(u.x, u.y));
						}
						finalMesh->indices.push_back(uniqueVertices[vertex]);
					}
				}
				indicesOffset += positions.count;
			}

			MeshRenderer* meshRenderer = new MeshRenderer(finalMesh, Application->activeScene->DefaultMaterial());
			meshRenderer->material = material;
			entity->AddComponent<MeshRenderer>(meshRenderer);
			verticesCount += finalMesh->vertices.size();
		}

		return Application->activeScene->GetEntity(mainEntity->uuid);
	}
}
