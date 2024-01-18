#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
namespace Plaza {
	enum RendererAPI;
	enum MeshType {
		Triangle = 0,
		HeightField
	};

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		bool isValid = true;
		~Vertex() = default;
		Vertex(const glm::vec3& pos)
			: Vertex(pos, glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)) {}

		Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex, const glm::vec3& tan, const glm::vec3& bitan)
			: position(pos), normal(norm), texCoords(tex), tangent(tan), bitangent(bitan) {}
		Vertex(){}
	};

	class Mesh {
	public:
		RendererAPI api;

		MeshType meshType = MeshType::Triangle;
		bool temporaryMesh = false;
		vector<glm::mat4> instanceModelMatrices = vector<glm::mat4>();
		uint64_t uuid;
		uint64_t meshId;
		float farthestVertex = 0.0f;
		std::string id;
		std::string meshName;
		uint64_t modelUuid;

		vector<glm::vec3> vertices;
		vector<unsigned int> indices;
		vector<glm::vec3> normals;
		vector<glm::vec2> uvs;
		vector<glm::vec3> tangent;
		vector<glm::vec3> bitangent;

		vector<Texture> textures;
		Material material = DefaultMaterial();
		bool usingNormal;
		glm::vec4 infVec = glm::vec4(INFINITY);

		virtual void setupMesh() {};
		virtual void Restart() {};
		virtual void Draw(Shader& shader) {};

		Material DefaultMaterial() {
			Material material;
			material.diffuse.rgba = glm::vec4(glm::pow(0.7f, 2.2f), glm::pow(0.7f, 2.2f), glm::pow(0.7f, 2.2f), glm::pow(0.7f, 2.2f));
			material.specular.rgba = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
			material.shininess = 2.0f;
			return material;
		}
	};
}

/*
	enum MeshType {
		Triangle = 0,
		HeightField
	};
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		bool isValid = true;
		~Vertex() = default;
		Vertex(const glm::vec3& pos)
			: Vertex(pos, glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)) {}

		Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex, const glm::vec3& tan, const glm::vec3& bitan)
			: position(pos), normal(norm), texCoords(tex), tangent(tan), bitangent(bitan) {}
	};

	class OpenGLMesh : public Mesh {
	public:
		MeshType meshType = MeshType::Triangle;
		bool temporaryMesh = false;
		unsigned int instanceBuffer;
		unsigned int uniformBuffer;
		vector<glm::mat4> instanceModelMatrices = vector<glm::mat4>();
		uint64_t uuid;
		uint64_t meshId;
		float farthestVertex = 0.0f;
		std::string id;
		std::string meshName;
		uint64_t modelUuid;

		//vector<Vertex> vertices;
		vector<glm::vec3> vertices;
		vector<unsigned int> indices;
		vector<glm::vec3> normals;
		vector<glm::vec2> uvs;
		vector<glm::vec3> tangent;
		vector<glm::vec3> bitangent;

		vector<Texture> textures;
		Material material = DefaultMaterial();
		bool usingNormal;
		glm::vec4 infVec = glm::vec4(INFINITY);
		unsigned int VAO;
*/