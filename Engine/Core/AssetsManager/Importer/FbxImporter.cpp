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
	static std::string GetFbxTexturePath(const ufbx_texture* texture, const std::string& materialFolderPath) {
		std::string path = texture->filename.data;
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

	static inline glm::vec3 ConvertUfbxVec3(ufbx_vec3 vec) {
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	static inline glm::vec2 ConvertUfbxVec2(ufbx_vec2 vec) {
		return glm::vec2(vec.x, vec.y);
	}

	static inline glm::quat ConvertUfbxQuat(ufbx_quat const& quaternionUfbx) {
		return glm::quat(quaternionUfbx.w, quaternionUfbx.x, quaternionUfbx.y, quaternionUfbx.z);
	};

	static glm::mat4 ConvertUfbxMatrix(ufbx_matrix const& mat4Ufbx) {
		glm::mat4 inverseBindMatrix = glm::identity<glm::mat4>();

		inverseBindMatrix[0][0] = mat4Ufbx.m00;
		inverseBindMatrix[0][1] = mat4Ufbx.m10;
		inverseBindMatrix[0][2] = mat4Ufbx.m20;

		inverseBindMatrix[1][0] = mat4Ufbx.m01;
		inverseBindMatrix[1][1] = mat4Ufbx.m11;
		inverseBindMatrix[1][2] = mat4Ufbx.m21;

		inverseBindMatrix[2][0] = mat4Ufbx.m02;
		inverseBindMatrix[2][1] = mat4Ufbx.m12;
		inverseBindMatrix[2][2] = mat4Ufbx.m22;

		inverseBindMatrix[3][0] = mat4Ufbx.m03;
		inverseBindMatrix[3][1] = mat4Ufbx.m13;
		inverseBindMatrix[3][2] = mat4Ufbx.m23;
		return inverseBindMatrix;
	};

	Entity* AssetsImporter::ImportFBX(AssetImported asset, std::filesystem::path outPath) {
		ufbx_load_opts opts = { };
		opts.target_axes = ufbx_axes_right_handed_y_up;
		opts.target_unit_meters = 1.0f;
		opts.target_axes = {
			.right = UFBX_COORDINATE_AXIS_POSITIVE_X,
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

		std::unordered_map<std::string, uint64_t> loadedTextures = std::unordered_map<std::string, uint64_t>();
		std::unordered_map<std::filesystem::path, uint64_t> loadedMaterials = std::unordered_map<std::filesystem::path, uint64_t>();

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
			for (ufbx_material* ufbxMaterial : node->materials) {

				std::filesystem::path materialOutPath = Editor::Gui::FileExplorer::currentDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(Editor::Gui::FileExplorer::currentDirectory + "\\" + ufbxMaterial->name.data) + Standards::materialExtName;
				bool materialIsNotLoaded = loadedMaterials.find(materialOutPath) == loadedMaterials.end();
				if (materialIsNotLoaded) {
					materialOutPath = Editor::Gui::FileExplorer::currentDirectory + "\\" + Editor::Utils::Filesystem::GetUnrepeatedName(Editor::Gui::FileExplorer::currentDirectory + "\\" + ufbxMaterial->name.data) + Standards::materialExtName;
					Material* material = AssetsImporter::FbxModelMaterialLoader(ufbxMaterial, std::filesystem::path{ asset.mPath }.parent_path().string(), loadedTextures);
					loadedMaterials.emplace(materialOutPath, material->uuid);
					AssetsSerializer::SerializeMaterial(material, materialOutPath);
					Application->activeScene->AddMaterial(material);
					materials.push_back(material);
				}
				else
					materials.push_back(Application->activeScene->GetMaterial(loadedMaterials.find(materialOutPath)->second));
				//materials.push_back(material);
			}

			/* Skinning */
			if (ufbxMesh->skin_deformers.count > 0) {
				ufbx_skin_deformer* skin = ufbxMesh->skin_deformers[0];
				for (unsigned int i = 0; i < skin->clusters.count; ++i) {
					ufbx_skin_cluster* cluster = skin->clusters[i];
					uint64_t parentUuid = 0;
					if (cluster->bone_node->parent)
						parentUuid = cluster->bone_node->bone->element_id;
					finalMesh->uniqueBonesInfo.emplace(cluster->bone_node->bone->element_id, Bone{ cluster->bone_node->bone->element_id, parentUuid, cluster->bone_node->bone->element_id, cluster->name.data, ConvertUfbxMatrix(cluster->geometry_to_bone) });
				}
			}

			uint64_t indices = 0;
			size_t triangleIndicesCount = ufbxMesh->max_face_triangles * 3;
			std::vector<uint32_t> triangleIndices = std::vector<uint32_t>();
			triangleIndices.resize(triangleIndicesCount);

			int faceIndex = 0;
			for (ufbx_face face : ufbxMesh->faces) {
				size_t trianglesNumber = ufbx_triangulate_face(triangleIndices.data(), triangleIndicesCount, ufbxMesh, face);
				uint32_t materialIndex = ufbxMesh->face_material[faceIndex];
				faceIndex++;
				for (size_t vertexIndex = 0; vertexIndex < trianglesNumber * 3; vertexIndex++) {
					uint32_t index = triangleIndices[vertexIndex];
					finalMesh->indices.push_back(index);


					glm::vec3 position = ConvertUfbxVec3(ufbxMesh->vertex_position[index]);
					glm::vec3 normal = ConvertUfbxVec3(ufbxMesh->vertex_normal[index]);
					glm::vec2 uv = ConvertUfbxVec2(ufbxMesh->vertex_uv[index]);

					finalMesh->vertices.push_back(position * mModelImporterScale);
					finalMesh->normals.push_back(normal);
					finalMesh->uvs.push_back(uv);

					finalMesh->materialsIndices.push_back(materialIndex);

					if (ufbxMesh->skin_deformers.count > 0) {
						ufbx_skin_deformer* skin = ufbxMesh->skin_deformers[0];
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
							totalWeight += skin_weight.weight;
						}


						if (totalWeight > 0)
						{
							for (uint32_t i = 0; i < num_weights; ++i)
							{
								holder.mWeights[i] /= totalWeight;
							}
						}
						finalMesh->bonesHolder.push_back(holder);
					}

					indices++;
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
			if (!finalMesh->materialsIndices.empty())
			{
				streams.push_back({ finalMesh->materialsIndices.data(), finalMesh->materialsIndices.size(), sizeof(finalMesh->materialsIndices[0]) });
			}

			finalMesh->indices.resize(ufbxMesh->num_triangles * 3);
			ufbx_error error;
			size_t num_vertices = ufbx_generate_indices(streams.data(), streams.size(), finalMesh->indices.data(), indices, NULL, &error);


			if (materials.size() <= 0)
				materials.push_back(Application->activeScene->DefaultMaterial());
			MeshRenderer* meshRenderer = new MeshRenderer(finalMesh, materials);
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
	}
}
