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

	struct BoundingBox {
		glm::vec4 maxVector = glm::vec4(glm::vec3(std::numeric_limits<float>::min()), 1.0f);
		glm::vec4 minVector = glm::vec4(glm::vec3(std::numeric_limits<float>::max()), 1.0f);
	};

	class Mesh {
	public:
		RendererAPI api;
		BoundingBox mBoundingBox{};

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

		//vector<Texture> textures;
		//Material material = DefaultMaterial();
		bool usingNormal;
		glm::vec4 infVec = glm::vec4(INFINITY);

		virtual void setupMesh() {};\
		virtual void Restart() {};
		virtual void Draw(Shader& shader) {};
		virtual void DrawInstances() {};

		void AddInstance(glm::mat4 model) {
			instanceModelMatrices.push_back(model);
		}

		void CalculateBoundingBox() {
			for (glm::vec3 vertex : this->vertices) {
				CalculateVertexInBoundingBox(vertex);
			}
		}

		void CalculateVertexInBoundingBox(glm::vec3 vertex) {
			mBoundingBox.minVector.x = std::min(mBoundingBox.minVector.x, vertex.x);
			mBoundingBox.minVector.y = std::min(mBoundingBox.minVector.y, vertex.y);
			mBoundingBox.minVector.z = std::min(mBoundingBox.minVector.z, vertex.z);

			mBoundingBox.maxVector.x = std::max(mBoundingBox.maxVector.x, vertex.x);
			mBoundingBox.maxVector.y = std::max(mBoundingBox.maxVector.y, vertex.y);
			mBoundingBox.maxVector.z = std::max(mBoundingBox.maxVector.z, vertex.z);
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