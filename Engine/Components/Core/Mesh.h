#pragma once
#include "Engine/Vendor/glad/glad.h"
#include "Engine/Vendor/glm/glm.hpp"
#include "Engine/Vendor/glm/gtc/matrix_transform.hpp"

#include "Engine/Shaders/Shader.h"

#include <string>
#include <vector>

#include "Engine/Components/Core/Material.h"
#include "Engine/Components/Core/Texture.h"
using namespace std;


namespace Engine {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoords;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		~Vertex() = default;
		Vertex(const glm::vec3& pos)
			: Vertex(pos, glm::vec3(0.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)) {}

		Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& tex, const glm::vec3& tan, const glm::vec3& bitan)
			: position(pos), normal(norm), texCoords(tex), tangent(tan), bitangent(bitan) {}
	};



	

	class Mesh {
	public:
		std::string id;
		vector<Vertex> vertices;
		vector<unsigned int> indices;
		vector<Texture> textures;
		Material material;
		glm::vec4 infVec = glm::vec4(INFINITY);
		unsigned int VAO;
		Mesh(const Mesh&) = default;
		~Mesh() = default;
		Mesh(vector<Vertex> vertices, vector<unsigned int> indices, Material material) {
			this->vertices = vertices;
			this->indices = indices;
			if (!material.diffuse.IsTextureEmpty()) {
				this->material.diffuse = Texture();
				this->material.diffuse.id = material.diffuse.id;
				this->material.diffuse.rgba = material.diffuse.rgba;
			}
			if (!material.specular.IsTextureEmpty()) {
				this->material.specular =Texture();
				this->material.specular.id = material.specular.id;
				this->material.specular.rgba = material.specular.rgba;
			}
			if (!material.normal.IsTextureEmpty()) {
				this->material.normal = Texture();
				this->material.normal.id = material.normal.id;
			}
			if (!material.height.IsTextureEmpty()) {
				this->material.height = Texture();
				this->material.height.id = material.height.id;
			}

			setupMesh();
		}

		void BindTextures(Shader& shader) {
			constexpr const char* shininessUniform = "shininess";
			constexpr const char* textureDiffuseRGBAUniform = "texture_diffuse_rgba";
			constexpr const char* textureDiffuseUniform = "texture_diffuse";
			constexpr const char* textureSpecularRGBAUniform = "texture_specular_rgba";
			constexpr const char* textureSpecularUniform = "texture_specular";
			constexpr const char* textureNormalUniform = "texture_normal";
			constexpr const char* textureHeightUniform = "texture_height";

			if (material.shininess != 64.0f) {
				shader.setFloat(shininessUniform, material.shininess);
			}

			if (!material.diffuse.IsTextureEmpty()) {
				const glm::vec4& diffuseRGBA = material.diffuse.rgba;
				if (diffuseRGBA != infVec) {
					shader.setVec4(textureDiffuseRGBAUniform, diffuseRGBA);
				}
				else {
					constexpr GLint textureDiffuseUnit = 0;
					glActiveTexture(GL_TEXTURE0 + textureDiffuseUnit);
					glBindTexture(GL_TEXTURE_2D, material.diffuse.id);
					shader.setInt(textureDiffuseUniform, textureDiffuseUnit);
					shader.setVec4(textureDiffuseRGBAUniform, glm::vec4(300, 300, 300, 300));
				}
			}

			if (!material.specular.IsTextureEmpty()) {
				const glm::vec4& specularRGBA = material.specular.rgba;
				if (specularRGBA != glm::vec4(INFINITY)) {
					shader.setVec4(textureSpecularRGBAUniform, specularRGBA);
				}
				else {
					constexpr GLint textureSpecularUnit = 1;
					glUniform1i(glGetUniformLocation(shader.ID, textureSpecularUniform), textureSpecularUnit);
					glActiveTexture(GL_TEXTURE0 + textureSpecularUnit);
					glBindTexture(GL_TEXTURE_2D, material.specular.id);
					shader.setVec4(textureSpecularRGBAUniform, glm::vec4(300, 300, 300, 300));
				}
			}

			if (!material.normal.IsTextureEmpty()) {
				constexpr GLint textureNormalUnit = 2;
				glUniform1i(glGetUniformLocation(shader.ID, textureNormalUniform), textureNormalUnit);
				glActiveTexture(GL_TEXTURE0 + textureNormalUnit);
				glBindTexture(GL_TEXTURE_2D, material.normal.id);
			}

			if (!material.height.IsTextureEmpty()) {
				constexpr GLint textureHeightUnit = 3;
				glUniform1i(glGetUniformLocation(shader.ID, textureHeightUniform), textureHeightUnit);
				glActiveTexture(GL_TEXTURE0 + textureHeightUnit);
				glBindTexture(GL_TEXTURE_2D, material.height.id);
			}
		}

		void Draw(Shader& shader) {
			// draw mesh
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// always good practice to set everything back to defaults once configured.
			glActiveTexture(GL_TEXTURE0);
		}

		static Mesh Cube() {
			std::vector<unsigned int> indices = {
0, 1, 2,  // Front face
2, 1, 3,  // Front face
4, 5, 6,  // Back face
6, 5, 7,  // Back face
8, 9, 10, // Top face
10, 9, 11,// Top face
12, 13, 14,// Bottom face
14, 13, 15,// Bottom face
16, 17, 18,// Left face
18, 17, 19,// Left face
20, 21, 22,// Right face
22, 21, 23 // Right face
			};
			std::vector<Vertex> vertices = {
				// Front face vertices
				Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Back face vertices
				Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Top face vertices
				Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Bottom face vertices
				Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Left face vertices
				Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				// Right face vertices
				Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
				Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0))
			};
			Material* cubeMaterial = new Material();
			cubeMaterial->diffuse = Texture();
			cubeMaterial->diffuse.type = "texture_diffuse";
			cubeMaterial->diffuse.rgba = glm::vec4(0.6f, 0.3f, 0.3f, 1.0f);

			Mesh newMesh = Mesh(vertices, indices, *cubeMaterial);
			delete cubeMaterial;
			return newMesh;
		}

		static Mesh Sphere() {
			std::vector<unsigned int> indices;
			std::vector<Vertex> vertices;

			// Generate sphere vertices and indices
			const int stacks = 20;
			const int slices = 40;
			const float radius = 1.0f;
			float PI = 3.14159265359f;

			for (int i = 0; i <= stacks; ++i) {
				float stackAngle = static_cast<float>(i) * PI / stacks;
				float stackRatio = static_cast<float>(i) / stacks;
				float phi = stackAngle - PI / 2.0f;

				for (int j = 0; j <= slices; ++j) {
					float sliceAngle = static_cast<float>(j) * 2.0f * PI / slices;
					float sliceRatio = static_cast<float>(j) / slices;

					float x = radius * std::cos(phi) * std::cos(sliceAngle);
					float y = radius * std::sin(phi);
					float z = radius * std::cos(phi) * std::sin(sliceAngle);

					glm::vec3 position(x, y, z);
					glm::vec3 normal = glm::normalize(position);
					glm::vec2 texCoords(sliceRatio, stackRatio);
					glm::vec3 tangent(0.0f);
					glm::vec3 bitangent(0.0f);

					vertices.push_back(Vertex(position, normal, texCoords, tangent, bitangent));
				}
			}

			// Generate sphere indices
			for (int i = 0; i < stacks; ++i) {
				int k1 = i * (slices + 1);
				int k2 = k1 + slices + 1;

				for (int j = 0; j < slices; ++j, ++k1, ++k2) {
					if (i != 0) {
						indices.push_back(k1);
						indices.push_back(k2);
						indices.push_back(k1 + 1);
					}

					if (i != stacks - 1) {
						indices.push_back(k1 + 1);
						indices.push_back(k2);
						indices.push_back(k2 + 1);
					}
				}
			}

			// Create sphere material
			Material* sphereMaterial = new Material();
			sphereMaterial->diffuse = Texture();
			sphereMaterial->diffuse.type = "texture_diffuse";
			sphereMaterial->diffuse.rgba = glm::vec4(0.6f, 0.3f, 0.3f, 1.0f);

			return Mesh(vertices, indices, *sphereMaterial);
		}

		static Mesh* Terrain(int width, int length, float scale, float amplitude) {
			std::vector<unsigned int> indices;
			std::vector<Vertex> vertices;

			// Generate plane vertices and indices
			for (int z = 0; z <= length; ++z) {
				for (int x = 0; x <= width; ++x) {
					float xPos = static_cast<float>(x) * scale;
					float zPos = static_cast<float>(z) * scale;
					float yPos = amplitude * std::sin(xPos / scale) * std::sin(zPos / scale);

					glm::vec3 position(xPos, yPos, zPos);
					glm::vec3 normal(0.0f, 1.0f, 0.0f);
					glm::vec2 texCoords(static_cast<float>(x) / width, static_cast<float>(z) / length);
					glm::vec3 tangent(0.0f);
					glm::vec3 bitangent(0.0f);

					vertices.push_back(Vertex(position, normal, texCoords, tangent, bitangent));
				}
			}

			// Generate plane indices
			for (int z = 0; z < length; ++z) {
				for (int x = 0; x < width; ++x) {
					int i1 = z * (width + 1) + x;
					int i2 = i1 + 1;
					int i3 = (z + 1) * (width + 1) + x;
					int i4 = i3 + 1;

					// Create triangles with reversed order of indices
					indices.push_back(i1);
					indices.push_back(i3);
					indices.push_back(i2);

					indices.push_back(i2);
					indices.push_back(i3);
					indices.push_back(i4);
				}
			}

			// Create terrain material
			Material* terrainMaterial = new Material();
			terrainMaterial->diffuse = Texture();
			terrainMaterial->diffuse.type = "texture_diffuse";
			terrainMaterial->diffuse.rgba = glm::vec4(0.6f, 0.3f, 0.3f, 1.0f);

			return new Mesh(vertices, indices, *terrainMaterial);
		}

	private:
		unsigned int VBO, EBO;
		void setupMesh() {
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
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

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
			glEnableVertexAttribArray(4);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
			glBindVertexArray(0);
		}
	};
}