#include "Engine/Components/Rendering/MeshRenderer.h"
namespace Engine {
	MeshRenderer::MeshRenderer(Engine::Mesh initialMesh, bool addToScene) {
		this->uuid = Engine::UUID::NewUUID();
		this->mesh = std::make_unique<Mesh>(initialMesh);
		if (addToScene)
			Application->activeScene->meshRenderers.emplace_back(this);
	}

	MeshRenderer::~MeshRenderer() {
		Application->activeScene->RemoveMeshRenderer(this->uuid);
	}
}