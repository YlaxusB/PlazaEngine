#include "AssetsImporter.h"
#include "ThirdParty/tinyobjloader/tiny_obj_loader.h"
#include "ThirdParty/glm/gtx/hash.hpp"
#include "Engine/Core/Renderer/Mesh.h"

struct Vec3Hash {
	std::size_t operator()(const glm::vec3& v) const {
		// Use glm's hash function for vec3
		return std::hash<glm::vec3>()(v);
	}
};


namespace Plaza {
	Entity* AssetsImporter::ImportOBJ(AssetImported asset, std::filesystem::path outPath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, asset.mPath.c_str())) {
			throw std::runtime_error(warn + err);
		}
		Entity* mainEntity = nullptr; //= new Entity()

		const std::vector<float>& positions = attrib.vertices;
		for (const auto& shape : shapes) {
			Entity* newEntity;
			if (!mainEntity)
			{
				newEntity = new Entity(shape.name, Application->activeScene->mainSceneEntity);
				mainEntity = newEntity;
			}
			else
				newEntity = new Entity(shape.name, mainEntity, true);

			std::vector<glm::vec3> vertices{};
			std::vector<glm::vec3> normals{};
			std::vector<glm::vec2> uvs{};
			std::vector<unsigned int> indices{};

			std::unordered_map<glm::vec3, uint32_t> uniqueVertices = std::unordered_map<glm::vec3, uint32_t>();
			for (size_t v = 0; v < shape.mesh.indices.size(); ++v) {
				tinyobj::index_t index = shape.mesh.indices[v];
				glm::vec3 vertex = glm::vec3(
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				) * mModelImporterScale;

				bool isVertexUnique = uniqueVertices.count(vertex) == 0;
				if (isVertexUnique) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());

					if (attrib.vertices.size() > 0) {
						vertices.push_back(vertex);
					}
					if (attrib.normals.size() > 0) {
						normals.push_back(glm::vec3(
							attrib.normals[3 * index.normal_index + 0],
							attrib.normals[3 * index.normal_index + 1],
							attrib.normals[3 * index.normal_index + 2]
						));
					}
					if (attrib.texcoords.size() > 0) {
						uvs.push_back(glm::vec2(
							attrib.texcoords[2 * max(index.texcoord_index, 0) + 0],
							attrib.texcoords[2 * max(index.texcoord_index, 0) + 1]
						));
					}
				}
				indices.push_back(uniqueVertices[vertex]);
			}

			Mesh& mesh = Application->mRenderer->CreateNewMesh(vertices, normals, uvs, std::vector<glm::vec3>(), std::vector<glm::vec3>(), indices, *Scene::DefaultMaterial(), false);// new Mesh();
			MeshRenderer* meshRenderer = new MeshRenderer(&mesh, Scene::DefaultMaterial());
			newEntity->AddComponent<MeshRenderer>(meshRenderer);
		}
		//AssetsSerializer::SerializePrefab(mainEntity, asset.mPath);
		return Application->activeScene->GetEntity(mainEntity->uuid);
	}
}