#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include <unordered_set>
#include <ThirdParty/cereal/cereal/types/string.hpp>
#include <ThirdParty/cereal/cereal/types/vector.hpp>
#include <ThirdParty/cereal/cereal/cereal.hpp>

namespace Plaza {
	enum RendererAPI;
	enum MeshType {
		Triangle = 0,
		HeightField
	};
	const unsigned int MAX_BONE_INFLUENCE = 4;
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		std::array<int, MAX_BONE_INFLUENCE> boneIds;
		std::array<float, MAX_BONE_INFLUENCE> weights;
		bool isValid;

		~Vertex() = default;

		Vertex(const glm::vec3& pos)
			: position(pos), normal(0.0f), texCoords(0.0f), tangent(0.0f), bitangent(0.0f), isValid(true) {
			for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
				boneIds[i] = -1;
				weights[i] = 0.0f;
			}
		}

		Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex, const glm::vec3& tan, const glm::vec3& bitan, const std::array<int, MAX_BONE_INFLUENCE> boneIds = std::array<int, MAX_BONE_INFLUENCE>(), const std::array<float, MAX_BONE_INFLUENCE> weights = std::array<float, MAX_BONE_INFLUENCE>())
			: position(pos), normal(norm), texCoords(tex), tangent(tan), bitangent(bitan), isValid(true), boneIds(boneIds), weights(weights) {
		}

		Vertex()
			: position(0.0f), normal(0.0f), texCoords(0.0f), tangent(0.0f), bitangent(0.0f), isValid(true) {
			for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
				boneIds[i] = -1;
				weights[i] = 0.0f;
			}
		}
	};

	struct Bone {
		uint64_t mId = -1;
		uint64_t mParentId = -1;
		std::string mName = "bone";
		Bone(uint64_t id, std::string name) : mId(id), mName(name) {};
		Bone(uint64_t id, uint64_t parentId, std::string name) : mId(id), mParentId(parentId), mName(name) {};
		float weight = 0.0f;
		glm::vec3 mPosition = glm::vec3(0.0f);
		glm::quat mRotation = glm::quat(glm::vec3(0.0f));
		glm::vec3 mScale = glm::vec3(1.0f);
		glm::mat4 mTransform = glm::mat4(1.0f);
		std::vector<uint64_t> mChildren = std::vector<uint64_t>();
		Bone() {};

		template <class Archive>
		void serialize(Archive& archive) {
			archive(mId, mName, weight, mPosition, mRotation, mScale, mParentId, mChildren);
		}
	};

	struct BonesHolder {
		std::vector<Bone> mBones = std::vector<Bone>();
		//std::vector<unsigned int> mBonesIds = std::vector<unsigned int>();

		std::array<int, MAX_BONE_INFLUENCE> GetBoneIds() {
			std::array<int, MAX_BONE_INFLUENCE> bonesArray = std::array<int, MAX_BONE_INFLUENCE>();
			for (unsigned int i = 0; i < mBones.size() && i < MAX_BONE_INFLUENCE; ++i) {
				bonesArray[i] = this->mBones[i].mId;
			}
			return bonesArray;
		}
		std::array<float, MAX_BONE_INFLUENCE> GetBoneWeights() {
			std::array<float, MAX_BONE_INFLUENCE> weightArray = std::array<float, MAX_BONE_INFLUENCE>();
			for (unsigned int i = 0; i < mBones.size() && i < MAX_BONE_INFLUENCE; ++i) {
				weightArray[i] = this->mBones[i].weight;
			}
			return weightArray;
		}

		template <class Archive>
		void serialize(Archive& archive) {
			archive(mBones);
		}
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
		vector<BonesHolder> bonesHolder;
		map<uint64_t, Bone> uniqueBonesInfo = map<uint64_t, Bone>();

		//vector<std::array<int, MAX_BONE_INFLUENCE>> boneIds;
		//vector<std::array<float, MAX_BONE_INFLUENCE>> weights;

		uint64_t verticesCount = 0;
		uint64_t verticesOffset = 0;
		uint64_t indicesCount = 0;
		uint64_t indicesOffset = 0;
		uint64_t instanceOffset = 0;

		//vector<Texture> textures;
		//Material material = DefaultMaterial();
		bool usingNormal;
		glm::vec4 infVec = glm::vec4(INFINITY);

		virtual void setupMesh() {};
		virtual void Restart() {};
		virtual void Draw(Shader& shader) {};
		virtual void DrawInstances() {};

		Mesh() {
			this->uuid = Plaza::UUID::NewUUID();
			this->meshId = this->uuid;
		}

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