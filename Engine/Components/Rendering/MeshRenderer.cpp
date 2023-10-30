#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/MeshRenderer.h"
#include "Engine/Components/Rendering/Mesh.h"

namespace Plaza {
	MeshRenderer::MeshRenderer(Plaza::Mesh initialMesh, bool addToScene) {
		this->uuid = Plaza::UUID::NewUUID();
		this->mesh = std::make_unique<Mesh>(initialMesh);
		if (addToScene)
			Application->activeScene->meshRenderers.emplace_back(this);
	}

	MeshRenderer::MeshRenderer(Plaza::Mesh initialMesh, Material material, bool addToScene) {
		this->uuid = Plaza::UUID::NewUUID();
		this->mesh = std::make_unique<Mesh>(initialMesh);
		this->material = std::make_unique<Material>(material);

		auto renderGroupIt = Application->activeScene->renderGroupsFindMap.find(std::make_pair(this->mesh->uuid, this->material->uuid));
		if (renderGroupIt != Application->activeScene->renderGroupsFindMap.end()) {
			this->renderGroup = Application->activeScene->renderGroups.at(renderGroupIt->second);
		}
		else {
			uint64_t renderGroupUuid = Plaza::UUID::NewUUID();
			Application->activeScene->renderGroups.emplace(renderGroupUuid, new RenderGroup(this->mesh, this->material));
			std::cout << "ok \n";
			uint64_t meshUuid = this->mesh->uuid;
			uint64_t materialUuid = this->material->uuid;
			Application->activeScene->renderGroupsFindMap.emplace(std::make_pair(meshUuid, materialUuid), renderGroupUuid);
			this->renderGroup = Application->activeScene->renderGroups.at(renderGroupUuid);
		}
		if (addToScene)
			Application->activeScene->meshRenderers.emplace_back(this);

	}

	MeshRenderer::~MeshRenderer() {
		Application->activeScene->RemoveMeshRenderer(this->uuid);
	}
}