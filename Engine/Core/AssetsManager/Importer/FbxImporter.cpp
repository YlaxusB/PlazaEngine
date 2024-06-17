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

	static std::string GetFbxTexturePath(const ofbx::Texture* texture, const std::string& materialFolderPath) {
		std::string path = toStringView(texture->getFileName());
		if (!std::filesystem::exists(path)) {
			path = std::filesystem::path{ materialFolderPath }.parent_path().string() + "\\" + toStringView(texture->getRelativeFileName());
			if (!std::filesystem::exists(path)) {
				path = materialFolderPath + "\\" + std::filesystem::path{ toStringView(texture->getFileName()) }.filename().string();
				if (!std::filesystem::exists(path)) {
					path = "";
				}
			}
		}

		return path;
	}
	Material* AssetsImporter::FbxModelMaterialLoader(const ofbx::Material* ofbxMaterial, const std::string materialFolderPath, std::unordered_map<std::string, uint64_t>& loadedTextures) {
		Material* material = new Material();
		material->name = ofbxMaterial->name;

		const ofbx::Texture* ofbxDiffuse = ofbxMaterial->getTexture(ofbx::Texture::DIFFUSE);
		if (ofbxDiffuse) {
			const std::string diffusePath = GetFbxTexturePath(ofbxDiffuse, materialFolderPath);//materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxDiffuse->getFileName()) }.filename().string();
			if (ofbxDiffuse && loadedTextures.find(diffusePath) == loadedTextures.end()) {
				material->diffuse = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(diffusePath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(diffusePath, material->diffuse->mAssetUuid);
			}
			else
				material->diffuse = AssetsManager::mTextures.at(loadedTextures.at(diffusePath));
		}

		const ofbx::Texture* ofbxNormal = ofbxMaterial->getTexture(ofbx::Texture::NORMAL);
		if (ofbxNormal) {
			const std::string normalPath = GetFbxTexturePath(ofbxNormal, materialFolderPath);//materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxNormal->getFileName()) }.filename().string();
			if (loadedTextures.find(normalPath) == loadedTextures.end()) {
				material->normal = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(normalPath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(normalPath, material->normal->mAssetUuid);
			}
			else
				material->normal = AssetsManager::mTextures.at(loadedTextures.at(normalPath));
		}

		const ofbx::Texture* ofbxSpecular = ofbxMaterial->getTexture(ofbx::Texture::SPECULAR);
		if (ofbxSpecular) {
			const std::string specularPath = GetFbxTexturePath(ofbxSpecular, materialFolderPath);;//materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxSpecular->getFileName()) }.filename().string();
			if (ofbxSpecular && loadedTextures.find(specularPath) == loadedTextures.end()) {
				material->roughness = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(specularPath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(specularPath, material->roughness->mAssetUuid);
			}
			else
				material->roughness = AssetsManager::mTextures.at(loadedTextures.at(specularPath));
		}

		const ofbx::Texture* ofbxReflection = ofbxMaterial->getTexture(ofbx::Texture::REFLECTION);
		if (ofbxReflection) {
			const std::string reflectionPath = GetFbxTexturePath(ofbxReflection, materialFolderPath);//materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxReflection->getFileName()) }.filename().string();
			if (ofbxReflection && loadedTextures.find(reflectionPath) == loadedTextures.end()) {
				material->metalness = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(reflectionPath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(reflectionPath, material->metalness->mAssetUuid);
			}
			else
				material->metalness = AssetsManager::mTextures.at(loadedTextures.at(reflectionPath));
		}
		return material;
	}

	int decodeIndex(int idx) {
		return (idx < 0) ? (-idx - 1) : idx;
	}

	Entity* AssetsImporter::ImportFBX(AssetImported asset, std::filesystem::path outPath) {
		FILE* fileOpen = fopen(asset.mPath.c_str(), "rb");

		if (!fileOpen) return nullptr;

		fseek(fileOpen, 0, SEEK_END);
		long fileSize = ftell(fileOpen);
		fseek(fileOpen, 0, SEEK_SET);
		auto* content = new ofbx::u8[fileSize];
		fread(content, 1, fileSize, fileOpen);

		//	ofbx::LoadFlags flags = 
				//		ofbx::LoadFlags::IGNORE_MODELS |
				//ofbx::LoadFlags::::IGNORE_CAMERAS |
				//ofbx::LoadFlags::IGNORE_LIGHTS |
				//		ofbx::LoadFlags::IGNORE_TEXTURES |
				//		ofbx::LoadFlags::IGNORE_MATERIALS |
				//ofbx::LoadFlags::IGNORE_VIDEOS;
			//		ofbx::LoadFlags::IGNORE_MESHES |;

		ofbx::IScene* loadedScene = ofbx::load((ofbx::u8*)content, fileSize, (ofbx::u64)ofbx::LoadFlags::TRIANGULATE);
		delete[] content;
		fclose(fileOpen);

		uint64_t verticesCount = 0;

		int meshCount = loadedScene->getMeshCount();

		// output unindexed geometry
		Entity* mainEntity = new Entity(loadedScene->getRoot()->name, Application->activeScene->mainSceneEntity);
		mainEntity->GetComponent<Transform>()->scale = glm::vec3(1.0f);
		std::unordered_map<ofbx::u64, uint64_t> meshIndexEntityMap = std::unordered_map<ofbx::u64, uint64_t>();
		std::unordered_map<uint64_t, Entity*> entities = std::unordered_map<uint64_t, Entity*>();
		std::unordered_map<uint64_t, ofbx::u64> entityMeshParent = std::unordered_map<uint64_t, ofbx::u64>();
		std::unordered_map<std::string, uint64_t> loadedTextures = std::unordered_map<std::string, uint64_t>();
		std::unordered_map<std::filesystem::path, uint64_t> loadedMaterials = std::unordered_map<std::filesystem::path, uint64_t>();
		std::unordered_map<ofbx::u64, uint64_t> materialsMap = std::unordered_map<ofbx::u64, uint64_t>();

		for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
			const ofbx::Mesh& mesh = *loadedScene->getMesh(meshIndex);

			std::vector<Material*> meshMaterials = std::vector<Material*>();

			for (int materialIndex = 0; materialIndex < mesh.getMaterialCount(); ++materialIndex) {
				std::filesystem::path materialOutPath = Editor::Gui::FileExplorer::currentDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(Editor::Gui::FileExplorer::currentDirectory + "\\" + mesh.getMaterial(materialIndex)->name) + Standards::materialExtName;
				bool materialIsNotLoaded = loadedMaterials.find(materialOutPath) == loadedMaterials.end();//materialsMap.find(mesh.getMaterial(materialIndex)->id) == materialsMap.end();
				if (materialIsNotLoaded) {
					Material* material = AssetsImporter::FbxModelMaterialLoader(mesh.getMaterial(materialIndex), std::filesystem::path{ asset.mPath }.parent_path().string(), loadedTextures);
					loadedMaterials.emplace(materialOutPath, material->uuid);
					materialsMap.emplace(mesh.getMaterial(materialIndex)->id, material->mAssetUuid);
					meshMaterials.push_back(material);
				}
			}

			//Material* material;
			//if (mesh.getMaterialCount() > 0) {
			//	const ofbx::Material* ofbxMaterial = mesh.getMaterial(0);
			//
			//	material = AssetsImporter::FbxModelMaterialLoader(ofbxMaterial, std::filesystem::path{ asset.mPath }.parent_path().string(), loadedTextures);
			//}
			//else {
			//	material = Scene::DefaultMaterial();
			//}

			//std::filesystem::path materialOutPath = Editor::Gui::FileExplorer::currentDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(Editor::Gui::FileExplorer::currentDirectory + "\\" + material->name) + Standards::materialExtName;
			//
			//if (loadedMaterials.find(materialOutPath) == loadedMaterials.end()) {
			//	loadedMaterials.emplace(materialOutPath, material->uuid);
			//	AssetsSerializer::SerializeMaterial(material, materialOutPath);
			//	Application->activeScene->AddMaterial(material);
			//}
			//else
			//	material = Application->activeScene->GetMaterial(loadedMaterials.find(materialOutPath)->second);

			Entity* entity = new Entity(mesh.name, mainEntity, true);
			const ofbx::Vec3 translation = mesh.getLocalTranslation();
			const glm::vec3 rotation = glm::vec3(mesh.getLocalRotation().x, mesh.getLocalRotation().y, mesh.getLocalRotation().z) * glm::vec3(glm::pi<float>() / 180.0f);
			entity->GetComponent<Transform>()->SetRelativePosition(glm::vec3(translation.x * mModelImporterScale.x, translation.y * mModelImporterScale.y, translation.z * mModelImporterScale.z));
			entity->GetComponent<Transform>()->SetRelativeRotation(glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z)));
			entity->GetComponent<Transform>()->SetRelativeScale(glm::vec3(mesh.getLocalScaling().x, mesh.getLocalScaling().y, mesh.getLocalScaling().z));

			entities.emplace(entity->uuid, entity);
			meshIndexEntityMap.emplace(mesh.id, entity->uuid);
			ofbx::Object* parent = mesh.getParent();
			if (parent) {
				entityMeshParent.emplace(entity->uuid, parent->id);
				auto parentIt = meshIndexEntityMap.find(parent->id);
				if (parentIt != meshIndexEntityMap.end()) {
					//entity->ChangeParent(Application->activeScene->GetEntity(entity->GetParent().uuid), Application->activeScene->GetEntity(parentIt->second));
				}
			}
			else {
				entityMeshParent.emplace(entity->uuid, 0);
			}
			const ofbx::Geometry* geometry = mesh.getGeometry();
			const ofbx::Vec3* positions = geometry->getVertices();
			const ofbx::Vec3* normals = geometry->getNormals();
			const ofbx::Vec2* uvs = geometry->getUVs();
			const int* faceIndicies = geometry->getFaceIndices();
			const int* meshTriangleMaterials = geometry->getMaterials();
			int indexCount = geometry->getIndexCount();

			Mesh* finalMesh = new Mesh();

			for (int j = 0; j < indexCount; ++j) {
				finalMesh->indices.push_back(decodeIndex(faceIndicies[j]));
			}

			/* TODO: FIX UNIQUE VERTICES, SO THEY ALSO DIFFER FROM NORMAL AND UV */
			std::unordered_map<glm::vec3, uint32_t> uniqueVertices = std::unordered_map<glm::vec3, uint32_t>();
			//int* tri_indices = new int();
			int indicesOffset = 0;

			std::unordered_map<uint64_t, std::vector<uint64_t>> vertexToBoneIDs;
			std::unordered_map<uint64_t, std::vector<float>> vertexToWeights;

			const ofbx::Skin* skin = mesh.getGeometry()->getSkin();
			if (skin) {
				const int clusterCount = skin->getClusterCount();
				for (int j = 0; j < clusterCount; ++j) {
					const ofbx::Cluster* cluster = skin->getCluster(j);
					if (cluster->getIndicesCount() <= 0)
						continue;
					const int* indices = cluster->getIndices();
					const double* weightsData = cluster->getWeights();

					finalMesh->uniqueBonesInfo.emplace(cluster->getLink()->id, Bone { cluster->getLink()->id, cluster->getLink()->getParent()->id, cluster->name });
					for (int k = 0; k < cluster->getIndicesCount(); ++k) {
						int vertexIndex = indices[k];
						uint64_t boneID = cluster->getLink()->id;
						float weight = static_cast<float>(weightsData[k]);

						vertexToBoneIDs[vertexIndex].push_back(boneID);
						vertexToWeights[vertexIndex].push_back(weight);
					}
				}
			}

			for (int j = 0; j < geometry->getVertexCount(); j++)
			{
				auto& v = geometry->getVertices()[j];
				auto& n = geometry->getNormals()[j];
				auto& t = geometry->getTangents()[j];
				auto& uv = geometry->getUVs()[j];

				Vertex vertex{};
				finalMesh->vertices.push_back(glm::vec3(v.x * mModelImporterScale.x, v.y * mModelImporterScale.y, v.z * mModelImporterScale.z));
				finalMesh->normals.push_back(glm::vec3(n.x, n.y, n.z));

				//if (geometry->getTangents() != nullptr) {
				//	vertex.tangent = glm::vec3(t.x, t.y, t.z);
				//	vertex.bitangent = glm::cross(vertex.tangent, vertex.normal);
				//}
				if (geometry->getUVs() != nullptr)
					finalMesh->uvs.push_back(glm::vec2(uv.x, 1.0f - uv.y));
				//finalMesh->indices.push_back(geometry->getFaceIndices()[j]);

				if (meshTriangleMaterials)
					finalMesh->materialsIndices.push_back(meshTriangleMaterials[j / 3]);
				else
					finalMesh->materialsIndices.push_back(0);

				if (vertexToBoneIDs.find(j) != vertexToBoneIDs.end()) {
					BonesHolder holder{};
					for (int k = 0; k < vertexToBoneIDs[j].size(); ++k) {
						holder.mBones.push_back(Bone{ vertexToBoneIDs[j][k], "" });
						holder.mWeights.push_back(vertexToWeights[j][k]);
					}
					if (vertexToBoneIDs[j].size() > 4)
						std::cout << "bones array bigger than 4 \n";
					finalMesh->bonesHolder.push_back(holder);
				}
				//vertex.blendingIndex[0] = -1;
				//vertex.blendingIndex[1] = -1;
				//vertex.blendingIndex[2] = -1;
				//vertex.blendingIndex[3] = -1;
				//vertex.blendingWeight = glm::vec4(0, 0, 0, 0);
				//vertices.push_back(vertex);
			}


			/*
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
						bool isVertexUnique = true;//uniqueVertices.count(vertex) == 0;
						if (isVertexUnique) {
							uniqueVertices[vertex] = static_cast<uint32_t>(finalMesh->vertices.size());
							finalMesh->vertices.push_back(glm::vec3(v.x, v.y, v.z) * mModelImporterScale);
							ofbx::Vec3 n = normals.get(tri_indices[i]);
							finalMesh->normals.push_back(glm::vec3(n.x, n.y, n.z));
							ofbx::Vec2 u = uvs.get(tri_indices[i]);
							finalMesh->uvs.push_back(glm::vec2(u.x, u.y));



							//std::vector<int> boneIDs = std::vector<int>();

							//const ofbx::Skin* skin = mesh.getGeometry()->getSkin();
							//const int clusterCount = skin->getClusterCount();
							//// Iterate over clusters
							//for (int j = 0; j < clusterCount; ++j) {
							//	const ofbx::Cluster* cluster = skin->getCluster(j);
							//	const int* indices = cluster->getIndices();

							//	// Check if this cluster affects the current vertex
							//	for (int k = 0; k < cluster->getIndicesCount(); ++k) {
							//		if (indices[k] == i) {
							//			// Add bone ID to the list
							//			if (boneIDs.size() < 4)
							//				boneIDs.push_back(cluster->getLink()->id);
							//			break;
							//		}
							//	}
							//}
							//finalMesh->bonesHolder = ;
						}
						std::vector<uint64_t> boneIDs = vertexToBoneIDs[tri_indices[i]];
						std::vector<float> weights = vertexToWeights[tri_indices[i]];
						const ofbx::Skin* skin = mesh.getGeometry()->getSkin();

						BonesHolder holder{};
						for (int i = 0; i < boneIDs.size(); ++i) {
							holder.mBones.push_back(Bone{ boneIDs[i], "" });
						}
						finalMesh->bonesHolder.push_back(holder);
						finalMesh->indices.push_back(uniqueVertices[vertex]);
					}
				}
				indicesOffset += positions.count;
			}
			*/
			/* Get bones */
			//const ofbx::Skin* te = mesh.getSkin();
			//int count = mesh.getSkin()->getClusterCount();
			//for (int i = 0; i < mesh.getSkin()->getClusterCount(); ++i) {
			//	const ofbx::Cluster* cluster = mesh.getSkin()->getCluster(i);
			//	const ofbx::Cluster* cluster2 = mesh.getSkin()->getCluster(i);
			//}
			//std::cout << " ---- BONES START ---- \n";
			//for (int i = 0, n = loadedScene->getAnimationStackCount(); i < n; ++i) {
			//	const ofbx::AnimationStack* stack = loadedScene->getAnimationStack(i);
			//	for (int j = 0; stack->getLayer(j); ++j) {
			//		const ofbx::AnimationLayer* layer = stack->getLayer(j);
			//		for (int k = 0; layer->getCurveNode(k); ++k) {
			//			const ofbx::AnimationCurveNode* node = layer->getCurveNode(k);
			//			if (node->getBone())
			//			{
			//				const ofbx::Object* bone = node->getBone();
			//
			//				Plaza::Bone plazaBone{ bone->id, bone->name };
			//				plazaBone.mParentId = bone->getParent()->id;
			//				if (finalMesh->uniqueBonesInfo.find(plazaBone.mId) == finalMesh->uniqueBonesInfo.end())
			//					finalMesh->uniqueBonesInfo.emplace(plazaBone.mId, plazaBone);
			//			};
			//		}
			//	}
			//}

			/* Reiterate to adjust parentship */
			for (int i = 0, n = loadedScene->getAnimationStackCount(); i < n; ++i) {
				const ofbx::AnimationStack* stack = loadedScene->getAnimationStack(i);
				for (int j = 0; stack->getLayer(j); ++j) {
					const ofbx::AnimationLayer* layer = stack->getLayer(j);
					for (int k = 0; layer->getCurveNode(k); ++k) {
						const ofbx::AnimationCurveNode* node = layer->getCurveNode(k);
						if (node->getBone())
						{
							const ofbx::Object* bone = node->getBone();
							uint64_t parentId = bone->getParent()->id;

							Plaza::Bone plazaBone{ bone->id, bone->name };
							if (finalMesh->uniqueBonesInfo.find(parentId) != finalMesh->uniqueBonesInfo.end())
								finalMesh->uniqueBonesInfo[parentId].mChildren.push_back(bone->id);
						};
					}
				}
			}

			//const ofbx::Cluster* cluster = mesh.gets()->getCluster(0);
			//const ofbx::Skin* skin = mesh.getSkin();
			finalMesh->bonesHolder.resize(finalMesh->indices.size());
			int count = 0;
			//if (skin) {
			//	for (unsigned int i = 0; i < skin->getClusterCount(); ++i) {
			//		const ofbx::Cluster* cluster = skin->getCluster(i);
			//		for (unsigned int j = 0; j < cluster->getIndicesCount(); ++j) {
			//			Plaza::Bone plazaBone{ cluster->getLink()->id, cluster->getLink()->getParent()->id, cluster->getLink()->name};
			//			if(finalMesh->uniqueBonesInfo.find(plazaBone.mId) == finalMesh->uniqueBonesInfo.end())
			//				finalMesh->uniqueBonesInfo.emplace(plazaBone.mId, plazaBone);
			//			std::cout << cluster->getIndices()[j] << "\n";
			//			finalMesh->bonesHolder[cluster->getIndices()[j]].mBones.push_back(plazaBone);
			//		}
			//		count += cluster->getIndicesCount();
			//		//insertHierarchy(m_bones, cluster->getLink());
			//	}
			//}



			MeshRenderer* meshRenderer = new MeshRenderer(finalMesh, meshMaterials);
			//meshRenderer->;//material = material;//AssetsLoader::LoadMaterial(AssetsManager::GetAsset(std::filesystem::path{ Editor::Gui::FileExplorer::currentDirectory + "\\" + material->name + Standards::materialExtName}.string()));
			entity->AddComponent<MeshRenderer>(meshRenderer);

			Collider* collider = new Collider();
			ColliderShape* shape = new ColliderShape(nullptr, Plaza::ColliderShape::MESH, finalMesh->uuid);
			collider->AddShape(shape);
			entity->AddComponent<Collider>(collider);
			verticesCount += finalMesh->vertices.size();
		}

		for (auto& [key, value] : entityMeshParent) {
			Entity* entity = &Application->activeScene->entities.at(key);
			if (value != 0 && meshIndexEntityMap.find(value) != meshIndexEntityMap.end()) {
				entity->ChangeParent(&entity->GetParent(), &Application->activeScene->entities.at(meshIndexEntityMap.at(value)));
			}
		}

		return Application->activeScene->GetEntity(mainEntity->uuid);
	}
}
