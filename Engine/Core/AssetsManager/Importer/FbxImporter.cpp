#include "Engine/Core/PreCompiledHeaders.h"
#include "AssetsImporter.h"
#include "ThirdParty/tinyobjloader/tiny_obj_loader.h"
#include "ThirdParty/glm/gtx/hash.hpp"
#include "Engine/Core/Renderer/Mesh.h"
#include "ThirdParty/OpenFBX/src/ofbx.h"


#include <d3d11.h>
#include <tchar.h>
#include <stdio.h>
#include <inttypes.h>
#include <vector>

struct Vec3Hash {
	std::size_t operator()(const glm::vec3& v) const {
		// Use glm's hash function for vec3
		return std::hash<glm::vec3>()(v);
	}
};


namespace Plaza {
	Mesh* AssetsImporter::ProcessMesh(aiMesh* aiMesh, const aiScene* scene, Material* material) {
		Mesh* mesh = new Mesh();

		mesh->vertices.resize(aiMesh->mNumVertices);
		mesh->indices.resize(aiMesh->mNumFaces);

		for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
		{
			glm::vec3 vertex;
			glm::vec3 normal;
			glm::vec3 tangent;
			glm::vec3 bitangent;
			glm::vec2 uv;

			// positions
			vertex.x = aiMesh->mVertices[i].x * mModelImporterScale.x;
			vertex.y = aiMesh->mVertices[i].y * mModelImporterScale.y;
			vertex.z = aiMesh->mVertices[i].z * mModelImporterScale.z;
			mesh->vertices.push_back(vertex);
			// normals
			if (aiMesh->HasNormals())
			{
				normal.x = aiMesh->mNormals[i].x * mModelImporterScale.x;
				normal.y = aiMesh->mNormals[i].y * mModelImporterScale.y;
				normal.z = aiMesh->mNormals[i].z * mModelImporterScale.z;
				mesh->normals.push_back(normal);
			}

			// texture coordinates
			if (aiMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
			{
				uv.x = aiMesh->mTextureCoords[0][i].x;
				uv.y = aiMesh->mTextureCoords[0][i].y;
				mesh->uvs.push_back(uv);
			}
			else
				uv = glm::vec2(0.0f, 0.0f);

			bool useTangent = false;
			if (aiMesh->HasTangentsAndBitangents() && useTangent) {
				//usingNormal = true;
				// tangent
				tangent.x = aiMesh->mTangents[i].x * mModelImporterScale.x;
				tangent.y = aiMesh->mTangents[i].y * mModelImporterScale.y;
				tangent.z = aiMesh->mTangents[i].z * mModelImporterScale.z;
				mesh->tangent.push_back(tangent);
				// bitangent
				bitangent.x = aiMesh->mBitangents[i].x * mModelImporterScale.x;
				bitangent.y = aiMesh->mBitangents[i].y * mModelImporterScale.y;
				bitangent.z = aiMesh->mBitangents[i].z * mModelImporterScale.z;
				mesh->bitangent.push_back(bitangent);
			}

		}
		// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
		{
			aiFace face = aiMesh->mFaces[i];
			// retrieve all indices of the face and store them in the indices vector
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				mesh->indices.push_back(face.mIndices[j]);
		}

		return mesh;
	}

	Entity* AssetsImporter::ProcessNode(aiNode* node, const aiScene* scene, Entity* parent) {
		Entity* entity = new Entity(node->mName.C_Str(), Application->activeScene->GetEntity(parent->uuid));
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh* aiMesh = scene->mMeshes[i];
			Material* material = new Material();
			Mesh* processedMesh = AssetsImporter::ProcessMesh(aiMesh, scene, material);

			MeshRenderer* meshRenderer = new MeshRenderer(processedMesh, Application->activeScene->DefaultMaterial());
			meshRenderer->renderGroup = Application->activeScene->AddRenderGroup(processedMesh, Application->activeScene->DefaultMaterial(), false);
			entity->AddComponent<MeshRenderer>(meshRenderer);


		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			Entity* child = AssetsImporter::ProcessNode(node->mChildren[i], scene, entity);
			child->ChangeParent(&child->GetParent(), Application->activeScene->GetEntity(entity->uuid));
		}

		return entity;
	}



	Entity* AssetsImporter::ImportFBX(AssetImported asset, std::filesystem::path outPath) {
		static char s_TimeString[256];
		FILE* fp = fopen(asset.mPath.c_str(), "rb");

		if (!fp) return nullptr;

		fseek(fp, 0, SEEK_END);
		long file_size = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		auto* content = new ofbx::u8[file_size];
		fread(content, 1, file_size, fp);

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

		ofbx::IScene* g_scene = ofbx::load((ofbx::u8*)content, file_size, (ofbx::u16)flags);
		return nullptr;
		//Assimp::Importer importer;
		//const aiScene* scene = importer.ReadFile(asset.mPath,
		//	aiProcess_JoinIdenticalVertices 
		//);
		//if (nullptr == scene) {
		//	cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		//	return nullptr;
		//}
		//
		//return Application->activeScene->GetEntity(AssetsImporter::ProcessNode(scene->mRootNode, scene, Application->activeScene->GetEntity(Application->activeScene->mainSceneEntity->uuid))->uuid);


		//if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, asset.mPath.c_str())) {
		//	throw std::runtime_error(warn + err);
		//}
		//Entity* mainEntity = nullptr; //= new Entity();
		//
		//const std::vector<float>& positions = attrib.vertices;
		//for (const auto& shape : shapes) {
		//	Entity* newEntity;
		//	if (!mainEntity)
		//	{
		//		newEntity = new Entity(shape.name, Application->activeScene->mainSceneEntity);
		//		mainEntity = newEntity;
		//	}
		//	else
		//		newEntity = new Entity(shape.name, mainEntity, true);
		//
		//	std::vector<glm::vec3> vertices{};
		//	std::vector<glm::vec3> normals{};
		//	std::vector<glm::vec2> uvs{};
		//	std::vector<unsigned int> indices{};
		//
		//	std::unordered_map<glm::vec3, uint32_t> uniqueVertices = std::unordered_map<glm::vec3, uint32_t>();
		//	for (size_t v = 0; v < shape.mesh.indices.size(); ++v) {
		//		tinyobj::index_t index = shape.mesh.indices[v];
		//		glm::vec3 vertex = glm::vec3(
		//			attrib.vertices[3 * index.vertex_index + 0],
		//			attrib.vertices[3 * index.vertex_index + 1],
		//			attrib.vertices[3 * index.vertex_index + 2]
		//		) * mModelImporterScale;
		//
		//		bool isVertexUnique = uniqueVertices.count(vertex) == 0;
		//		if (isVertexUnique) {
		//			uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
		//
		//			if (attrib.vertices.size() > 0) {
		//				vertices.push_back(vertex);
		//			}
		//			if (attrib.normals.size() > 0) {
		//				normals.push_back(glm::vec3(
		//					attrib.normals[3 * index.normal_index + 0],
		//					attrib.normals[3 * index.normal_index + 1],
		//					attrib.normals[3 * index.normal_index + 2]
		//				));
		//			}
		//			if (attrib.texcoords.size() > 0) {
		//				uvs.push_back(glm::vec2(
		//					attrib.texcoords[2 * index.texcoord_index + 0],
		//					attrib.texcoords[2 * index.texcoord_index + 1]
		//				));
		//			}
		//		}
		//		indices.push_back(uniqueVertices[vertex]);
		//	}
		//
		//	Mesh& mesh = Application->mRenderer->CreateNewMesh(vertices, normals, uvs, std::vector<glm::vec3>(), std::vector<glm::vec3>(), indices, *Scene::DefaultMaterial(), false);// new Mesh();
		//	MeshRenderer* meshRenderer = new MeshRenderer(mesh, Scene::DefaultMaterial());
		//	newEntity->AddComponent<MeshRenderer>(meshRenderer);
		//}
		//AssetsSerializer::SerializePrefab(mainEntity, asset.mPath);
		//return Application->activeScene->GetEntity(mainEntity->uuid);
	}
}