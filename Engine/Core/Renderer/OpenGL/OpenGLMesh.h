#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Component.h"
#include "Engine/Core/Renderer/Texture.h"
#include "Engine/Components/Rendering/Material.h"
#include "Engine/Core/Time.h"

#include "Engine/Core/Renderer/Mesh.h"
#include "Renderer.h"

using namespace std;
using namespace Plaza;
namespace Plaza {
	class OpenGLMesh : public Mesh {
	public:
		RendererAPI api = RendererAPI::OpenGL;

		unsigned int instanceBuffer;
		unsigned int uniformBuffer;
		unsigned int VAO;
		~OpenGLMesh() {
			if (temporaryMesh) {
				if (VAO)
					glDeleteVertexArrays(1, &VAO);
				if (VBO)
					glDeleteBuffers(1, &VBO);
				if (EBO)
					glDeleteBuffers(1, &EBO);
			}
		};

		OpenGLMesh(const OpenGLMesh&) = default;
		OpenGLMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> uvs, vector<glm::vec3> tangent, vector<unsigned int> indices) {
			this->vertices = vertices;
			this->indices = indices;
			this->normals = normals;
			this->uvs = uvs;
			this->tangent = tangent;
			this->uuid = Plaza::UUID::NewUUID();
			if (this->meshId == 0)
				this->meshId = Plaza::UUID::NewUUID();
			setupMesh();
		}

		//OpenGLMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> uvs, vector<glm::vec3> tangent, vector<glm::vec3> bitangent, vector<unsigned int> indices) {
		//	this->vertices = vertices;
		//	this->indices = indices;
		//	this->normals = normals;
		//	this->uvs = uvs;
		//	this->tangent = tangent;
		//	this->bitangent = bitangent;
		//	this->indices = indices;
		//	this->uuid = Plaza::UUID::NewUUID();
		//	if (this->meshId == 0)
		//		this->meshId = Plaza::UUID::NewUUID();
		//	setupMesh();
		//}

		OpenGLMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> uvs, vector<glm::vec3> tangent, vector<unsigned int> indices, bool usingNormal) {
			this->vertices = vertices;
			this->indices = indices;
			this->normals = normals;
			this->uvs = uvs;
			this->tangent = tangent;
			this->indices = indices;
			this->usingNormal = usingNormal;
			this->uuid = Plaza::UUID::NewUUID();
			if (this->meshId == 0)
				this->meshId = Plaza::UUID::NewUUID();
			setupMesh();
		}

		OpenGLMesh() {
			uuid = Plaza::UUID::NewUUID();
		}

		void AddInstance(Shader& shader, glm::mat4 model) {
			//Time::addInstanceCalls += 1;
			instanceModelMatrices.push_back(model);
		}

		void Terminate() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);
			//glDeleteBuffers(1, &EBO);
		}

		void Restart() override {
			if (indices.size() > 0 && vertices.size() > 0) {
				/*			glDeleteBuffers(1, &VBO);
							glDeleteBuffers(1, &EBO);
							glDeleteBuffers(1, &instanceBuffer);
							glDeleteBuffers(1, &this->uniformBuffer);
							glDeleteVertexArrays(1, &VAO);*/
							//setupMesh();
				// Update vertices
				if (VBO && EBO) {
					glBindBuffer(GL_ARRAY_BUFFER, VBO);
					glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_DYNAMIC_DRAW);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(unsigned int), &this->indices[0], GL_STATIC_DRAW);

					// Assuming that normals and UVs are part of the Vertex struct
					// If not, adjust the offsets accordingly
					// vertex normals
					glEnableVertexAttribArray(1);
					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

					// vertex texture coords
					glEnableVertexAttribArray(2);
					glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

					// Note: Assuming tangent and bitangent are also part of the Vertex struct
					// If not, adjust the offsets accordingly
					// vertex tangent
					glEnableVertexAttribArray(3);
					glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

					//// vertex bitangent
					//glEnableVertexAttribArray(4);
					//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

					// Update normals and UVs
					glBindBuffer(GL_ARRAY_BUFFER, VBO);

					// Assuming normals are stored after the position in Vertex struct
					glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3), this->normals.size() * sizeof(glm::vec3), &this->normals[0]);

					// Assuming UVs are stored after the position and normals in Vertex struct
					glBufferSubData(GL_ARRAY_BUFFER, sizeof(glm::vec3) + sizeof(glm::vec3), this->uvs.size() * sizeof(glm::vec2), &this->uvs[0]);

					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
				else {
					setupMesh();
				}
			}

		}
		void Init()
		{

		}

	private:
		unsigned int VBO, EBO;
		void setupMesh() override {
			// create buffers/arrays
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);
			// load data into vertex buffers
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// A great thing about structs is that their memory layout is sequential for all its items.
			// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
			// again translates to 3/2 floats which translates to a byte array.
			vector<Vertex> convertedVertices;
			convertedVertices.reserve(vertices.size()); // Reserve space for better performance

			//;; TODO: FIX OPENGL SETUP MESH
			for (unsigned int i = 0; i < vertices.size(); i++) {
				//convertedVertices.push_back(Vertex{
				//	vertices[i],
				//	(normals.size() > i) ? normals[i] : glm::vec3(0.0f),
				//	(uvs.size() > i) ? uvs[i] : glm::vec2(0.0f),
				//	(tangent.size() > i) ? tangent[i] : glm::vec3(0.0f),
				//	(bitangent.size() > i) ? bitangent[i] : glm::vec3(0.0f),
				//	(materialsIndices.size() > i) ? materialsIndices[i] : -1
				//	//(bones.size() > i && bones[i].mId != -1) ? bones[i].mId : std::array<int, MAX_BONE_INFLUENCE>(),
				//	//(weights.size() > i) ? weights[i] : std::array<float, MAX_BONE_INFLUENCE>()
				//	});
			}
			glBufferData(GL_ARRAY_BUFFER, convertedVertices.size() * sizeof(Vertex), &convertedVertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

			// set the vertex attribute pointers
			// vertex Positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
			//vertex tangent
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
			// vertex bitangent
			//glEnableVertexAttribArray(4);
			//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));


			// Generate instanced array
			glGenBuffers(1, &instanceBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, instanceBuffer);
			// set attribute pointers for matrix (4 times vec4)
			glEnableVertexAttribArray(7);
			glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);


			glEnableVertexAttribArray(8);
			glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));

			glEnableVertexAttribArray(9);
			glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));


			glEnableVertexAttribArray(10);
			glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));


			glVertexAttribDivisor(7, 1);
			glVertexAttribDivisor(8, 1);
			glVertexAttribDivisor(9, 1);
			glVertexAttribDivisor(10, 1);
			glBindVertexArray(0);

			// Generate Uniform Buffer
			glGenBuffers(1, &this->uniformBuffer);
			glBindBuffer(GL_UNIFORM_BUFFER, this->uniformBuffer);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), new Material(), GL_STATIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
			for (glm::vec3 position : vertices) {
				glm::vec3 absoluteVertex = glm::vec3(glm::abs(position.x), glm::abs(position.y), glm::abs(position.z));
				float absoluteSum = absoluteVertex.x + absoluteVertex.y + absoluteVertex.z;
				if (absoluteSum > farthestVertex)
					farthestVertex = absoluteSum;
			}
		}

	};
}