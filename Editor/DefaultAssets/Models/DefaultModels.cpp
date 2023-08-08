#include "Engine/Core/PreCompiledHeaders.h"
#include "DefaultModels.h"
#include "Engine/Components/Core/Mesh.h"
namespace Engine::Editor {
	vector<Mesh*> DefaultModels::meshes = vector<Mesh*>();
	void DefaultModels::Init() {
		InitCube();
	}
	shared_ptr<Mesh> DefaultModels::Cube() {
		return Application->editorScene->meshes.front();//make_shared<Mesh>(*(new Mesh(*Application->editorScene->meshes.front())));
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
		delete cubeMaterial;
		Application->editorScene->meshes.push_back(make_shared<Mesh>(*newMesh));
		//meshes.push_back(newMesh);
		//Application->editorScene->meshes.push_back(new Mesh(*meshes.front()));
	}
}