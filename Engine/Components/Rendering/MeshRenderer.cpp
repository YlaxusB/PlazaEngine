#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
namespace Plaza {
	MeshRenderer::MeshRenderer(Plaza::Mesh initialMesh, bool addToScene) {
		this->uuid = Plaza::UUID::NewUUID();
		this->mesh = std::make_unique<Mesh>(initialMesh);
		if (addToScene)
			Application->activeScene->meshRenderers.emplace_back(this);
	}

	MeshRenderer::~MeshRenderer() {
		Application->activeScene->RemoveMeshRenderer(this->uuid);
	}
}