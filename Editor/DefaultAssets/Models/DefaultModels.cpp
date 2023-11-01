#include "Engine/Core/PreCompiledHeaders.h"
#include "DefaultModels.h"
#include "Engine/Components/Rendering/Mesh.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
namespace Plaza::Editor {

	vector<Mesh*> DefaultModels::meshes = vector<Mesh*>();
	uint64_t DefaultModels::cubeUuid = 1;
	uint64_t DefaultModels::sphereUuid = 2;
	uint64_t DefaultModels::planeUuid = 3;
	uint64_t DefaultModels::cylinderUuid = 4;

	void DefaultModels::Init() {
		InitCube();
		InitSphere();
		InitPlane();
		InitCylinder();
		//Material defaultMaterial = Material();
		//defaultMaterial.albedo.rgba = glm::vec4(1.0f);
		//defaultMaterial.diffuse.rgba = glm::vec4(1.0f);
		//defaultMaterial.uuid = -1;
		////Application->activeScene->materials.emplace(-1, std::make_shared<Material>(defaultMaterial));
		//Application->activeScene->AddMaterial(&defaultMaterial);
	}
	shared_ptr<Mesh> DefaultModels::Cube() {
		return Application->editorScene->meshes.at(cubeUuid);
	}
	shared_ptr<Mesh> DefaultModels::Sphere() {
		return Application->editorScene->meshes.at(sphereUuid);
	}
	shared_ptr<Mesh> DefaultModels::Plane() {
		return Application->editorScene->meshes.at(planeUuid);
	}

	shared_ptr<Mesh> DefaultModels::Cylinder() {
		return Application->editorScene->meshes.at(cylinderUuid);
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
		std::vector<glm::vec3> positions = {
			glm::vec3(-0.5f, -0.5f, 0.5f),glm::vec3(0.5f, -0.5f, 0.5f),glm::vec3(-0.5f, 0.5f, 0.5f),glm::vec3(0.5f, 0.5f, 0.5f),
			glm::vec3(0.5f, -0.5f, -0.5f),glm::vec3(-0.5f, -0.5f, -0.5f),glm::vec3(0.5f, 0.5f, -0.5f),glm::vec3(-0.5f, 0.5f, -0.5f),
			glm::vec3(-0.5f, 0.5f, 0.5f),glm::vec3(0.5f, 0.5f, 0.5f),glm::vec3(-0.5f, 0.5f, -0.5f),glm::vec3(0.5f, 0.5f, -0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.5f, -0.5f, -0.5f),glm::vec3(-0.5f, -0.5f, 0.5f),glm::vec3(0.5f, -0.5f, 0.5f),
			glm::vec3(-0.5f, -0.5f, -0.5f),glm::vec3(-0.5f, -0.5f, 0.5f),glm::vec3(-0.5f, 0.5f, -0.5f),glm::vec3(-0.5f, 0.5f, 0.5f),
			glm::vec3(0.5f, -0.5f, 0.5f),glm::vec3(0.5f, -0.5f, -0.5f),glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.5f, 0.5f, -0.5f)
		};

		std::vector<glm::vec3> normals = {
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(0, 0, -1),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(0, -1, 0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, 0, 0),
			glm::vec3(-1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0),
			glm::vec3(1, 0, 0)
		};

		std::vector<glm::vec2> texCoords = {
			glm::vec2(0, 0), glm::vec2(1, 0),
			glm::vec2(0, 1), glm::vec2(1, 1),
			glm::vec2(0, 0),glm::vec2(1, 0),
			glm::vec2(0, 1),glm::vec2(1, 1),
			glm::vec2(0, 0), glm::vec2(1, 0),
			glm::vec2(0, 1),glm::vec2(1, 1),
			glm::vec2(0, 0),glm::vec2(1, 0),
			glm::vec2(0, 1),glm::vec2(1, 1),
			glm::vec2(0, 0),glm::vec2(1, 0),
			glm::vec2(0, 1),glm::vec2(1, 1),
			glm::vec2(0, 0),glm::vec2(1, 0),
			glm::vec2(0, 1),glm::vec2(1, 1)
		};

		//std::vector<Vertex> vertices = {
		//	// Front face vertices
		//	Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	// Back face vertices
		//	Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	// Top face vertices
		//	Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	// Bottom face vertices
		//	Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	// Left face vertices
		//	Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	// Right face vertices
		//	Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)),
		//	Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0))
		//};
		Mesh* newMesh = new Mesh(positions, normals, texCoords, vector<glm::vec3>(), vector<glm::vec3>(), indices);
		newMesh->usingNormal = false;
		newMesh->meshId = cubeUuid;
		Application->editorScene->meshes.emplace(newMesh->meshId, make_shared<Mesh>(*newMesh));
	}

	void DefaultModels::InitSphere() {
		std::vector<unsigned int> indices;
		//std::vector<Vertex> vertices;
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<glm::vec2> uvs;

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

				vertices.push_back(glm::vec3(x, y, z));
				normals.push_back(glm::normalize(glm::vec3(x, y, z)));
				uvs.push_back(glm::vec2(sliceRatio, stackRatio));
				tangents.push_back(glm::vec3(0.0f));
				bitangents.push_back(glm::vec3(0.0f));

				//vertices.push_back(Vertex(position, normal, texCoords, tangent, bitangent));
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

		shared_ptr<Mesh> newMesh = make_shared<Mesh>(vertices, normals, uvs, tangents, bitangents, indices);
		newMesh->usingNormal = false;
		newMesh->meshId = sphereUuid;
		Application->editorScene->meshes.emplace(newMesh->meshId, newMesh);
	}

	void DefaultModels::InitPlane() {
		std::vector<unsigned int> indices = {
			0, 1, 2,
			2, 3, 0
		};

		std::vector<glm::vec3> vertices = {
			glm::vec3(-0.5f, 0.0f, 0.5f),
			glm::vec3(0.5f, 0.0f, 0.5f),
			glm::vec3(0.5f, 0.0f, -0.5f),
			glm::vec3(-0.5f, 0.0f, -0.5f)
		};

		std::vector<glm::vec3> normals = {
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0)
		};

		std::vector<glm::vec2> texCoords = {
			glm::vec2(0, 0),
			glm::vec2(1, 0),
			glm::vec2(1, 1),
			glm::vec2(0, 1)
		};

		Mesh* newMesh = new Mesh(vertices, normals, texCoords, vector<glm::vec3>(), vector<glm::vec3>(), indices);
		newMesh->usingNormal = false;
		newMesh->meshId = planeUuid;
		Application->editorScene->meshes.emplace(newMesh->meshId, make_shared<Mesh>(*newMesh));
	}

	void DefaultModels::InitCylinder() {
		float radius = 0.5f;
		float height = 1.0f;
		int slices = 20;
		int stacks = 20;
		vector<Vertex> vertices;
		vector<unsigned int> indices;

		float pi = 3.14159265359;
		//Vertex(glm::vec3(x, y, z), glm::vec3(0, 0, 1), glm::vec2(s, 1 - i / stacks), glm::vec3(0.0f), glm::vec3(0.0f))
		for (unsigned int i = 1; i < slices; i++) {
			float x = glm::cos((slices / i)) * radius;
			float z = glm::sin((slices / i)) * radius;
			float y = 0;
			vertices.push_back(Vertex(glm::vec3(x, y, z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)));
			vertices.push_back(Vertex(glm::vec3(x, y + height, z), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f), glm::vec3(0.0f), glm::vec3(0.0f)));
		}

		for (unsigned int i = 0; i < slices; i += 2) {
			indices.push_back(i);
			indices.push_back(i + slices / 2);
			indices.push_back(i + 1);

			indices.push_back(i + 1);
			indices.push_back(i + slices / 2);
			indices.push_back(i + slices / 2 + 1);

			//indices.push_back(i);
			//indices.push_back(i + slices + 1);
			//indices.push_back(i + slices + 2);
		}

		// Create the cylinder mesh and add it to the scene
		//Mesh* newMesh = new Mesh(vertices, indices);
		//newMesh->usingNormal = false; // You probably want to set this to true for the cylinder
		//newMesh->meshId = cylinderUuid;
		//Application->editorScene->meshes.emplace(newMesh->meshId, std::make_shared<Mesh>(*newMesh));
	}
}