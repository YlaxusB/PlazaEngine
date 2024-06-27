#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsImporter.h"
#include "ThirdParty/tinyobjloader/tiny_obj_loader.h"
#include "ThirdParty/glm/gtx/hash.hpp"
#include "Engine/Core/Renderer/Mesh.h"
#include "Engine/Core/AssetsManager/Serializer/AssetsSerializer.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "ThirdParty/ufbx/ufbx/ufbx.c"

#include <stdio.h>
#include <d3d11.h>
#include <tchar.h>
#include <stdio.h>
#include <inttypes.h>
#include <vector>
#include <malloc.h>

struct Vec3Hash {
	std::size_t operator()(const glm::vec3& v) const {
		return std::hash<glm::vec3>()(v);
	}
};


namespace Plaza {
	//static std::string toStringView(ofbx::DataView data) {
	//	return std::string(
	//		(const char*)data.begin,
	//		(const char*)data.end
	//	);
	//}

	static std::string GetFbxTexturePath(const ufbx_texture* texture, const std::string& materialFolderPath) {
		std::string path = texture->name.data;
		if (!std::filesystem::exists(path)) {
			path = std::filesystem::path{ materialFolderPath }.parent_path().string() + "\\" + texture->relative_filename.data;
			if (!std::filesystem::exists(path)) {
				path = materialFolderPath + "\\" + std::filesystem::path{ texture->absolute_filename.data }.filename().string();
				if (!std::filesystem::exists(path)) {
					path = "";
				}
			}
		}

		return path;
	}
	Material* AssetsImporter::FbxModelMaterialLoader(const ufbx_material* ufbxMaterial, const std::string materialFolderPath, std::unordered_map<std::string, uint64_t>& loadedTextures) {
		Material* material = new Material();
		material->name = ufbxMaterial->name.data;

		ufbx_texture* ofbxDiffuse = ufbxMaterial->pbr.base_color.texture;//getTexture(UFBX_SHADER_FBX_LAMBERT ofbx::Texture::DIFFUSE);
		if (ofbxDiffuse) {
			const std::string diffusePath = GetFbxTexturePath(ofbxDiffuse, materialFolderPath);//materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxDiffuse->getFileName()) }.filename().string();
			if (ofbxDiffuse && loadedTextures.find(diffusePath) == loadedTextures.end()) {
				material->diffuse = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(diffusePath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(diffusePath, material->diffuse->mAssetUuid);
			}
			else
				material->diffuse = AssetsManager::mTextures.at(loadedTextures.at(diffusePath));
		}

		ufbx_texture* ofbxNormal = ufbxMaterial->pbr.normal_map.texture;//ufbxMaterial->getTexture(ofbx::Texture::NORMAL);
		if (ofbxNormal) {
			const std::string normalPath = GetFbxTexturePath(ofbxNormal, materialFolderPath);//materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxNormal->getFileName()) }.filename().string();
			if (loadedTextures.find(normalPath) == loadedTextures.end()) {
				material->normal = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(normalPath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(normalPath, material->normal->mAssetUuid);
			}
			else
				material->normal = AssetsManager::mTextures.at(loadedTextures.at(normalPath));
		}

		ufbx_texture* ofbxSpecular = ufbxMaterial->pbr.roughness.texture;//ofbxMaterial->getTexture(ofbx::Texture::SPECULAR);
		if (ofbxSpecular) {
			const std::string specularPath = GetFbxTexturePath(ofbxSpecular, materialFolderPath);;//materialFolderPath + "\\" + std::filesystem::path{ toStringView(ofbxSpecular->getFileName()) }.filename().string();
			if (ofbxSpecular && loadedTextures.find(specularPath) == loadedTextures.end()) {
				material->roughness = AssetsLoader::LoadTexture(AssetsManager::GetAssetOrImport(specularPath, Plaza::UUID::NewUUID()));
				loadedTextures.emplace(specularPath, material->roughness->mAssetUuid);
			}
			else
				material->roughness = AssetsManager::mTextures.at(loadedTextures.at(specularPath));
		}

		ufbx_texture* ofbxReflection = ufbxMaterial->pbr.metalness.texture;//ofbxMaterial->getTexture(ofbx::Texture::REFLECTION);
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

	//static glm::mat4 ofbxToGlm(const ofbx::Matrix& ofbxMat, glm::vec3 scale) {
	//	glm::mat4 glmMat = glm::mat4(1.0f);
	//
	//	//// Copy elements from ofbx::Matrix to glm::mat4
	//	//for (int i = 0; i < 4; ++i) {
	//	//	for (int j = 0; j < 4; ++j) {
	//	//		glmMat[i][j] = static_cast<float>(ofbxMat.m[i * 4 + j]);
	//	//	}
	//	//}
	//
	//	for (int row = 0; row < 4; ++row) {
	//		for (int col = 0; col < 4; ++col) {
	//			double v = ofbxMat.m[4 * col + row];
	//			glmMat[col][row] = v;
	//		}
	//	}
	//
	//	return glmMat;
	//}

	static inline glm::vec3 ConvertUfbxVec3(ufbx_vec3 vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::vec2 ConvertUfbxVec2(ufbx_vec2 vec) {
		return glm::vec2(vec.x, vec.y);
	}

	static inline glm::quat ConvertUfbxQuat(ufbx_quat const& quaternionUfbx) {
		glm::vec4 vec4GLM;
		vec4GLM.x = quaternionUfbx.x;
		vec4GLM.y = quaternionUfbx.y;
		vec4GLM.z = quaternionUfbx.z;
		vec4GLM.w = quaternionUfbx.w;
		return glm::quat(vec4GLM);
	};

	static glm::mat4 ConvertUfbxMatrix(ufbx_matrix const& mat4Ufbx) {
		glm::mat4 mat4Glm;
		for (unsigned int column = 0; column < 4; ++column)
		{
			mat4Glm[column].x = mat4Ufbx.cols[column].x;
			mat4Glm[column].y = mat4Ufbx.cols[column].y;
			mat4Glm[column].z = mat4Ufbx.cols[column].z;
			mat4Glm[column].w = column < 3 ? 0.0f : 1.0f;
		}
		return mat4Glm;
	};

	Entity* AssetsImporter::ImportFBX(AssetImported asset, std::filesystem::path outPath) {
		ufbx_load_opts opts = { };
		opts.target_axes = ufbx_axes_right_handed_y_up,
		opts.target_unit_meters = 1.0f;
		opts.target_axes = {
			.right = UFBX_COORDINATE_AXIS_NEGATIVE_X,
			.up = UFBX_COORDINATE_AXIS_POSITIVE_Y,
			.front = UFBX_COORDINATE_AXIS_POSITIVE_Z,
		};

		ufbx_error error;
		ufbx_scene* scene = ufbx_load_file(asset.mPath.c_str(), &opts, &error);

		if (!scene) {
			fprintf(stderr, "Failed to load scene: %s\n", error.description.data);
			return nullptr;
		}

		Entity* mainEntity = new Entity(scene->root_node->name.data, Application->activeScene->mainSceneEntity);

		std::unordered_map<uint64_t, Entity*> entities = std::unordered_map<uint64_t, Entity*>();
		std::unordered_map<uint64_t, uint64_t> meshIndexEntityMap = std::unordered_map<uint64_t, uint64_t>(); // ufbx id, plaza uuid
		std::unordered_map<uint64_t, uint64_t> entityMeshParent = std::unordered_map<uint64_t, uint64_t>(); // Entity, Parent

		for (size_t i = 0; i < scene->nodes.count; ++i) {
			ufbx_node* node = scene->nodes.data[i];
			ufbx_mesh* ufbxMesh = node->mesh;
			if (!ufbxMesh)
				continue;
			ufbx_transform& transform = node->local_transform;

			Entity* entity = new Entity(ufbxMesh->name.data, mainEntity, true);
			entity->GetComponent<Transform>()->SetRelativePosition(ConvertUfbxVec3(transform.translation));
			entity->GetComponent<Transform>()->SetRelativeRotation(ConvertUfbxQuat(transform.rotation));
			entity->GetComponent<Transform>()->SetRelativeScale(ConvertUfbxVec3(transform.scale));

			entities.emplace(entity->uuid, entity);
			meshIndexEntityMap.emplace(ufbxMesh->element_id, entity->uuid);
			ufbx_node* parentNode = node->parent;
			if (parentNode)
				entityMeshParent.emplace(entity->uuid, parentNode->element_id);
			else
				entityMeshParent.emplace(entity->uuid, 0);


			Mesh* finalMesh = new Mesh();

			/* Material */
			std::vector<Material*> materials = std::vector<Material*>();
			std::unordered_map<std::string, uint64_t> loadedTextures = std::unordered_map<std::string, uint64_t>();
			std::unordered_map<std::filesystem::path, uint64_t> loadedMaterials = std::unordered_map<std::filesystem::path, uint64_t>();
			for (ufbx_material* ufbxMaterial : node->materials) {
				Material* material = AssetsImporter::FbxModelMaterialLoader(ufbxMaterial, std::filesystem::path{ asset.mPath }.parent_path().string(), loadedTextures);

				std::filesystem::path materialOutPath = Editor::Gui::FileExplorer::currentDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(Editor::Gui::FileExplorer::currentDirectory + "\\" + material->name) + Standards::materialExtName;

				if (loadedMaterials.find(materialOutPath) == loadedMaterials.end()) {
					loadedMaterials.emplace(materialOutPath, material->uuid);
					AssetsSerializer::SerializeMaterial(material, materialOutPath);
					Application->activeScene->AddMaterial(material);
				}
				else
					material = Application->activeScene->GetMaterial(loadedMaterials.find(materialOutPath)->second);
				materials.push_back(material);
			}

			/* Skinning */
			ufbx_skin_deformer* skin = ufbxMesh->skin_deformers[0];
			if (skin)
			{
				for (unsigned int i = 0; i < skin->clusters.count; ++i) {
					ufbx_skin_cluster* cluster = skin->clusters[i];
					uint64_t parentUuid = 0;
					if (cluster->bone_node->parent)
						parentUuid = cluster->bone_node->bone->element_id;
					finalMesh->uniqueBonesInfo.emplace(cluster->bone_node->bone->element_id, Bone{ cluster->bone_node->bone->element_id, parentUuid, cluster->bone_node->bone->element_id, cluster->name.data, ConvertUfbxMatrix(cluster->bone_node->geometry_to_node) });
				}
			}

			//if (skin)
			//{
			//	for (unsigned int i = 0; i < skin->clusters.count; ++i) {
			//		ufbx_skin_cluster* cluster = skin->clusters[i];
			//		for (ufbx_node* child : cluster->bone_node->children) {
			//			finalMesh->uniqueBonesInfo[child->bone->element_id].mParentId = cluster->bone_node->bone->element_id;
			//			finalMesh->uniqueBonesInfo[cluster->bone_node->bone->element_id].mChildren.push_back(child->bone->element_id);
			//
			//		}
			//
			//		//if (cluster->bone_node->parent && cluster->bone_node->parent->bone && finalMesh->uniqueBonesInfo.find(cluster->bone_node->parent->bone->element_id) != finalMesh->uniqueBonesInfo.end()) {
			//		//	finalMesh->uniqueBonesInfo[cluster->bone_node->bone->element_id].mParentId = cluster->bone_node->parent->bone->element_id;
			//		//	finalMesh->uniqueBonesInfo[cluster->bone_node->parent->bone->element_id].mChildren.push_back(cluster->bone_node->bone->element_id);
			//		//}
			//	}
			//}

			uint64_t indices = 0;
			size_t triangleIndicesCount = ufbxMesh->max_face_triangles * 3;
			std::vector<uint32_t> triangleIndices = std::vector<uint32_t>();
			triangleIndices.resize(triangleIndicesCount);

			for (ufbx_face face : ufbxMesh->faces) {
				size_t trianglesNumber = ufbx_triangulate_face(triangleIndices.data(), triangleIndicesCount, ufbxMesh, face);

				for (size_t vertexIndex = 0; vertexIndex < trianglesNumber * 3; vertexIndex++) {
					uint32_t index = triangleIndices[vertexIndex];
					finalMesh->indices.push_back(index);


					glm::vec3 position = ConvertUfbxVec3(ufbxMesh->vertex_position[index]);
					glm::vec3 normal = ConvertUfbxVec3(ufbxMesh->vertex_normal[index]);
					glm::vec2 uv = ConvertUfbxVec2(ufbxMesh->vertex_uv[index]);

					finalMesh->vertices.push_back(position * mModelImporterScale);
					finalMesh->normals.push_back(normal);
					finalMesh->uvs.push_back(uv);

					//finalMesh->materialsIndices.push_back(ufbxMesh->face_material[index]);

					if (ufbxMesh->face_material.count > index)
						finalMesh->materialsIndices.push_back(ufbxMesh->face_material[index]);
					else
						finalMesh->materialsIndices.push_back(0);
					ufbx_skin_deformer* skin = ufbxMesh->skin_deformers[0];
					if (skin)
					{
						const uint32_t vertex = ufbxMesh->vertex_indices[index];


						ufbx_skin_vertex skinVertex = skin->vertices[vertex];
						uint32_t num_weights = skinVertex.num_weights;
						num_weights = std::min(num_weights, 4u);

						float totalWeight = 0;
						Plaza::BonesHolder holder{};
						for (uint32_t i = 0; i < num_weights; ++i)
						{
							ufbx_skin_weight skin_weight = skin->weights[skinVertex.weight_begin + i];
							holder.mBones.push_back(skin->clusters[skin_weight.cluster_index]->bone_node->bone->element_id);
							holder.mWeights.push_back(skin_weight.weight);
							//(&boneindices.back().x)[i] = skin_weight.cluster_index;
							//(&boneweights.back().x)[i] = skin_weight.weight;
							totalWeight += skin_weight.weight;
						}


						if (totalWeight > 0)
						{
							for (uint32_t i = 0; i < num_weights; ++i)
							{
								//(&boneweights.back().x)[i] /= totalWeight;
								holder.mWeights[i] /= totalWeight;
							}
						}
						finalMesh->bonesHolder.push_back(holder);
					}



					indices++;
					//		if (meshTriangleMaterials)
//			finalMesh->materialsIndices.push_back(meshTriangleMaterials[j / 3]);
//		else
//			finalMesh->materialsIndices.push_back(0);

//		if (vertexToBoneIDs.find(j) != vertexToBoneIDs.end()) {
//			BonesHolder holder{};
//			for (int k = 0; k < vertexToBoneIDs[j].size(); ++k) {
//				holder.mBones.push_back(vertexToBoneIDs[j][k]);
//				holder.mWeights.push_back(vertexToWeights[j][k]);
//			}
//			if (vertexToBoneIDs[j].size() > 4)
//				std::cout << "bones array bigger than 4 \n";
//			finalMesh->bonesHolder.push_back(holder);
//		}
				}
			}
			vector<ufbx_vertex_stream> streams;
			if (!finalMesh->vertices.empty())
			{
				streams.push_back({ finalMesh->vertices.data(), finalMesh->vertices.size(), sizeof(finalMesh->vertices[0]) });
			}
			if (!finalMesh->normals.empty())
			{
				streams.push_back({ finalMesh->normals.data(), finalMesh->normals.size(), sizeof(finalMesh->normals[0]) });
			}
			if (!finalMesh->uvs.empty())
			{
				streams.push_back({ finalMesh->uvs.data(), finalMesh->uvs.size(), sizeof(finalMesh->uvs[0]) });
			}
			if (!finalMesh->bonesHolder.empty())
			{
				streams.push_back({ finalMesh->bonesHolder.data(), finalMesh->bonesHolder.size(), sizeof(finalMesh->bonesHolder[0]) });
			}
			//if (!boneweights.empty())
			//{
			//	streams.push_back({ boneweights.data(), boneweights.size(), sizeof(boneweights[0]) });
			//}

			finalMesh->indices.resize(ufbxMesh->num_triangles * 3);
			ufbx_error error;
			size_t num_vertices = ufbx_generate_indices(streams.data(), streams.size(), finalMesh->indices.data(), indices, NULL, &error);


			if (materials.size() <= 0)
				materials.push_back(Application->activeScene->DefaultMaterial());
			MeshRenderer* meshRenderer = new MeshRenderer(finalMesh, materials);
			//meshRenderer->;//material = material;//AssetsLoader::LoadMaterial(AssetsManager::GetAsset(std::filesystem::path{ Editor::Gui::FileExplorer::currentDirectory + "\\" + material->name + Standards::materialExtName}.string()));
			entity->AddComponent<MeshRenderer>(meshRenderer);

			Collider* collider = new Collider();
			ColliderShape* shape = new ColliderShape(nullptr, Plaza::ColliderShape::MESH, finalMesh->uuid);
			collider->AddShape(shape);
			entity->AddComponent<Collider>(collider);
		}

		for (auto& [key, value] : entityMeshParent) {
			Entity* entity = &Application->activeScene->entities.at(key);
			if (value != 0 && meshIndexEntityMap.find(value) != meshIndexEntityMap.end()) {
				entity->ChangeParent(&entity->GetParent(), &Application->activeScene->entities.at(meshIndexEntityMap.at(value)));
			}
		}

		ufbx_free_scene(scene);
		return Application->activeScene->GetEntity(mainEntity->uuid);

		//uint64_t verticesCount = 0;

		//int meshCount = loadedScene->getMeshCount();

		//// output unindexed geometry
		//Entity* mainEntity = new Entity(loadedScene->getRoot()->name, Application->activeScene->mainSceneEntity);
		//mainEntity->GetComponent<Transform>()->scale = glm::vec3(1.0f);
		//std::unordered_map<ofbx::u64, uint64_t> meshIndexEntityMap = std::unordered_map<ofbx::u64, uint64_t>();
		//std::unordered_map<uint64_t, Entity*> entities = std::unordered_map<uint64_t, Entity*>();
		//std::unordered_map<std::string, uint64_t> loadedTextures = std::unordered_map<std::string, uint64_t>();
		//std::unordered_map<std::filesystem::path, uint64_t> loadedMaterials = std::unordered_map<std::filesystem::path, uint64_t>();
		//std::unordered_map<ofbx::u64, uint64_t> materialsMap = std::unordered_map<ofbx::u64, uint64_t>();

		//for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
		//	const ofbx::Mesh& mesh = *loadedScene->getMesh(meshIndex);

		//	std::vector<Material*> meshMaterials = std::vector<Material*>();

		//	for (int materialIndex = 0; materialIndex < mesh.getMaterialCount(); ++materialIndex) {
		//		std::filesystem::path materialOutPath = Editor::Gui::FileExplorer::currentDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(Editor::Gui::FileExplorer::currentDirectory + "\\" + mesh.getMaterial(materialIndex)->name) + Standards::materialExtName;
		//		bool materialIsNotLoaded = loadedMaterials.find(materialOutPath) == loadedMaterials.end();//materialsMap.find(mesh.getMaterial(materialIndex)->id) == materialsMap.end();
		//		if (materialIsNotLoaded) {
		//			Material* material = AssetsImporter::FbxModelMaterialLoader(mesh.getMaterial(materialIndex), std::filesystem::path{ asset.mPath }.parent_path().string(), loadedTextures);
		//			loadedMaterials.emplace(materialOutPath, material->uuid);
		//			materialsMap.emplace(mesh.getMaterial(materialIndex)->id, material->mAssetUuid);
		//			meshMaterials.push_back(material);

		//			AssetsSerializer::SerializeMaterial(material, materialOutPath);
		//			Application->activeScene->AddMaterial(material);
		//		}
		//	}

		//	Entity* entity = new Entity(mesh.name, mainEntity, true);
		//	const ofbx::Vec3 translation = mesh.getLocalTranslation();
		//	const glm::vec3 rotation = glm::vec3(mesh.getLocalRotation().x, mesh.getLocalRotation().y, mesh.getLocalRotation().z) * glm::vec3(glm::pi<float>() / 180.0f);
		//	entity->GetComponent<Transform>()->SetRelativePosition(glm::vec3(translation.x * mModelImporterScale.x, translation.y * mModelImporterScale.y, translation.z * mModelImporterScale.z));
		//	entity->GetComponent<Transform>()->SetRelativeRotation(glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z)));
		//	entity->GetComponent<Transform>()->SetRelativeScale(glm::vec3(mesh.getLocalScaling().x, mesh.getLocalScaling().y, mesh.getLocalScaling().z));

		//	entities.emplace(entity->uuid, entity);
		//	meshIndexEntityMap.emplace(mesh.id, entity->uuid);
		//	ofbx::Object* parent = mesh.getParent();
		//	if (parent) {
		//		entityMeshParent.emplace(entity->uuid, parent->id);
		//		auto parentIt = meshIndexEntityMap.find(parent->id);
		//		if (parentIt != meshIndexEntityMap.end()) {
		//			//entity->ChangeParent(Application->activeScene->GetEntity(entity->GetParent().uuid), Application->activeScene->GetEntity(parentIt->second));
		//		}
		//	}
		//	else {
		//		entityMeshParent.emplace(entity->uuid, 0);
		//	}
		//	const ofbx::Geometry* geometry = mesh.getGeometry();
		//	const ofbx::Vec3* positions = geometry->getVertices();
		//	const ofbx::Vec3* normals = geometry->getNormals();
		//	const ofbx::Vec2* uvs = geometry->getUVs();
		//	const int* faceIndicies = geometry->getFaceIndices();
		//	const int* meshTriangleMaterials = geometry->getMaterials();
		//	int indexCount = geometry->getIndexCount();

		//	Mesh* finalMesh = new Mesh();

		//	for (int j = 0; j < indexCount; ++j) {
		//		finalMesh->indices.push_back(decodeIndex(faceIndicies[j]));
		//	}

		//	/* TODO: FIX UNIQUE VERTICES, SO THEY ALSO DIFFER FROM NORMAL AND UV */
		//	std::unordered_map<glm::vec3, uint32_t> uniqueVertices = std::unordered_map<glm::vec3, uint32_t>();
		//	//int* tri_indices = new int();
		//	int indicesOffset = 0;

		//	std::unordered_map<uint64_t, std::vector<uint64_t>> vertexToBoneIDs;
		//	std::unordered_map<uint64_t, std::vector<float>> vertexToWeights;

		//	const ofbx::Skin* skin = mesh.getGeometry()->getSkin();
		//	if (skin) {
		//		const int clusterCount = skin->getClusterCount();
		//		for (int j = 0; j < clusterCount; ++j) {
		//			const ofbx::Cluster* cluster = skin->getCluster(j);
		//			if (cluster->getIndicesCount() <= 0)
		//				continue;
		//			const int* indices = cluster->getIndices();
		//			const double* weightsData = cluster->getWeights();
		//			finalMesh->uniqueBonesInfo.emplace(cluster->getLink()->id, Bone { cluster->getLink()->id, cluster->getLink()->getParent()->id, cluster->name, (ofbxToGlm(cluster->getTransformMatrix(), mModelImporterScale)) });
		//			for (int k = 0; k < cluster->getIndicesCount(); ++k) {
		//				int vertexIndex = indices[k];
		//				uint64_t boneID = cluster->getLink()->id;
		//				float weight = static_cast<float>(weightsData[k]);

		//				vertexToBoneIDs[vertexIndex].push_back(boneID);
		//				vertexToWeights[vertexIndex].push_back(weight);
		//			}
		//		}
		//	}

		//	for (int j = 0; j < geometry->getVertexCount(); j++)
		//	{
		//		auto& v = geometry->getVertices()[j];
		//		auto& n = geometry->getNormals()[j];
		//		auto& t = geometry->getTangents()[j];
		//		auto& uv = geometry->getUVs()[j];

		//		Vertex vertex{};
		//		finalMesh->vertices.push_back(glm::vec3(v.x * mModelImporterScale.x, v.y * mModelImporterScale.y, v.z * mModelImporterScale.z));
		//		finalMesh->normals.push_back(glm::vec3(n.x, n.y, n.z));

		//		//if (geometry->getTangents() != nullptr) {
		//		//	vertex.tangent = glm::vec3(t.x, t.y, t.z);
		//		//	vertex.bitangent = glm::cross(vertex.tangent, vertex.normal);
		//		//}
		//		if (geometry->getUVs() != nullptr)
		//			finalMesh->uvs.push_back(glm::vec2(uv.x, 1.0f - uv.y));
		//		//finalMesh->indices.push_back(geometry->getFaceIndices()[j]);

		//		if (meshTriangleMaterials)
		//			finalMesh->materialsIndices.push_back(meshTriangleMaterials[j / 3]);
		//		else
		//			finalMesh->materialsIndices.push_back(0);

		//		if (vertexToBoneIDs.find(j) != vertexToBoneIDs.end()) {
		//			BonesHolder holder{};
		//			for (int k = 0; k < vertexToBoneIDs[j].size(); ++k) {
		//				holder.mBones.push_back(vertexToBoneIDs[j][k]);
		//				holder.mWeights.push_back(vertexToWeights[j][k]);
		//			}
		//			if (vertexToBoneIDs[j].size() > 4)
		//				std::cout << "bones array bigger than 4 \n";
		//			finalMesh->bonesHolder.push_back(holder);
		//		}

		//	}


		//	/* Reiterate to adjust parentship */
		//	for (int i = 0, n = loadedScene->getAnimationStackCount(); i < n; ++i) {
		//		const ofbx::AnimationStack* stack = loadedScene->getAnimationStack(i);
		//		for (int j = 0; stack->getLayer(j); ++j) {
		//			const ofbx::AnimationLayer* layer = stack->getLayer(j);
		//			for (int k = 0; layer->getCurveNode(k); ++k) {
		//				const ofbx::AnimationCurveNode* node = layer->getCurveNode(k);
		//				if (node->getBone())
		//				{
		//					const ofbx::Object* bone = node->getBone();
		//					uint64_t parentId = bone->getParent()->id;

		//					Plaza::Bone plazaBone{ bone->id, bone->name };
		//					if (finalMesh->uniqueBonesInfo.find(parentId) != finalMesh->uniqueBonesInfo.end() && finalMesh->uniqueBonesInfo.find(bone->id) != finalMesh->uniqueBonesInfo.end()) {
		//						finalMesh->uniqueBonesInfo[parentId].mChildren.push_back(bone->id);
		//						finalMesh->uniqueBonesInfo[bone->id].mOffset = finalMesh->uniqueBonesInfo[parentId].mOffset * finalMesh->uniqueBonesInfo[bone->id].mOffset;
		//					}
		//				};
		//			}
		//		}
		//	}


		//	finalMesh->bonesHolder.resize(finalMesh->indices.size());


		//	MeshRenderer* meshRenderer = new MeshRenderer(finalMesh, meshMaterials);
		//	//meshRenderer->;//material = material;//AssetsLoader::LoadMaterial(AssetsManager::GetAsset(std::filesystem::path{ Editor::Gui::FileExplorer::currentDirectory + "\\" + material->name + Standards::materialExtName}.string()));
		//	entity->AddComponent<MeshRenderer>(meshRenderer);

		//	Collider* collider = new Collider();
		//	ColliderShape* shape = new ColliderShape(nullptr, Plaza::ColliderShape::MESH, finalMesh->uuid);
		//	collider->AddShape(shape);
		//	entity->AddComponent<Collider>(collider);
		//	verticesCount += finalMesh->vertices.size();
		//}
	}
}
