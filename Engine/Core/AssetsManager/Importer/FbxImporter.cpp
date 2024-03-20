#include "Engine/Core/PreCompiledHeaders.h"
#include <ThirdParty/OpenFBX/src/libdeflate.h>
#include <ThirdParty/OpenFBX/src/ofbx.h>
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
	Entity* AssetsImporter::ImportFBX(AssetImported asset, std::filesystem::path outPath) {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		static char s_TimeString[256];
		FILE* fileOpen = fopen(outPath.string().c_str(), "rb");
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

		FILE* fp = fopen(outPath.string().c_str(), "wb");
		if (!fp) return nullptr;
		int indicesOffset = 0;
		int meshCount = loadedScene->getMeshCount();

		for (int meshIndex = 0; meshIndex < meshCount; ++meshIndex) {
			const ofbx::Mesh& mesh = *loadedScene->getMesh(meshIndex);
			const ofbx::GeometryData& geom = mesh.getGeometryData();
			const ofbx::Vec3Attributes positions = geom.getPositions();
			const ofbx::Vec3Attributes normals = geom.getNormals();
			const ofbx::Vec2Attributes uvs = geom.getUVs();
			
			// each ofbx::Mesh can have several materials == partitions
			for (int partitionIndex = 0; partitionIndex < geom.getPartitionCount(); ++partitionIndex) {
				fprintf(fp, "o obj%d_%d\ng grp%d\n", meshIndex, partitionIndex, meshIndex);
				const ofbx::GeometryPartition& partition = geom.getPartition(partitionIndex);

				// partitions most likely have several polygons, they are not triangles necessarily, use ofbx::triangulate if you want triangles
				for (int polygonIndex = 0; polygonIndex < partition.polygon_count; ++polygonIndex) {
					const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygonIndex];

					for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
						ofbx::Vec3 v = positions.get(i);
						fprintf(fp, "v %f %f %f\n", v.x, v.y, v.z);
					}

					bool has_normals = normals.values != nullptr;
					if (has_normals) {
						// normals.indices might be different than positions.indices
						// but normals.get(i) is normal for positions.get(i)
						for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
							ofbx::Vec3 n = normals.get(i);
							fprintf(fp, "vn %f %f %f\n", n.x, n.y, n.z);
						}
					}

					bool has_uvs = uvs.values != nullptr;
					if (has_uvs) {
						for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
							ofbx::Vec2 uv = uvs.get(i);
							fprintf(fp, "vt %f %f\n", uv.x, uv.y);
						}
					}
				}

				for (int polygonIndex = 0; polygonIndex < partition.polygon_count; ++polygonIndex) {
					const ofbx::GeometryPartition::Polygon& polygon = partition.polygons[polygonIndex];
					fputs("f ", fp);
					for (int i = polygon.from_vertex; i < polygon.from_vertex + polygon.vertex_count; ++i) {
						fprintf(fp, "%d ", 1 + i + indicesOffset);
					}
					fputs("\n", fp);
				}

				indicesOffset += positions.count;
			}
		}

		fclose(fp);
		return nullptr;



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