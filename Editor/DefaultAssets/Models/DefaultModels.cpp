#include "Engine/Core/PreCompiledHeaders.h"
#include "DefaultModels.h"
#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
namespace Engine::Editor {

	vector<Mesh*> DefaultModels::meshes = vector<Mesh*>();
	void DefaultModels::Init() {
		InitCube();
		InitSphere();
		InitPlane();
	}
	shared_ptr<Mesh> DefaultModels::Cube() {
		return Application->editorScene->meshes.front();
	}
	shared_ptr<Mesh> DefaultModels::Sphere() {
		return Application->editorScene->meshes.at(1);
	}
	shared_ptr<Mesh> DefaultModels::Plane() {
		return Application->editorScene->meshes.at(2);
	}

	void DefaultModels::InitCube() {
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
		cubeMaterial->diffuse.rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);
		Mesh* newMesh = new Mesh(vertices, indices, *cubeMaterial);
		newMesh->usingNormal = false;
		delete cubeMaterial;
		Application->editorScene->meshes.push_back(make_shared<Mesh>(*newMesh));
	}

	void DefaultModels::InitSphere() {
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
		sphereMaterial->diffuse.rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);

		Mesh* newMesh = new Mesh(vertices, indices, *sphereMaterial);
		newMesh->usingNormal = false;
		delete sphereMaterial;
		Application->editorScene->meshes.push_back(make_shared<Mesh>(*newMesh));
	}

	void DefaultModels::InitPlane() {
		std::vector<unsigned int> indices = {
			0, 1, 2,
			2, 3, 0
		};

		std::vector<Vertex> vertices = {
			Vertex(glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(0), glm::vec3(0)),
			Vertex(glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0), glm::vec3(0)),
			Vertex(glm::vec3(0.5f, 0.0f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(0), glm::vec3(0)),
			Vertex(glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0), glm::vec3(0))
		};

		// Create sphere material
		Material* material = new Material();
		material->diffuse = Texture();
		material->diffuse.type = "texture_diffuse";
		material->diffuse.rgba = glm::vec4(0.8f, 0.3f, 0.3f, 1.0f);

		Mesh* newMesh = new Mesh(vertices, indices, *material);
		newMesh->usingNormal = false;
		delete material;
		Application->editorScene->meshes.push_back(make_shared<Mesh>(*newMesh));
	}
}