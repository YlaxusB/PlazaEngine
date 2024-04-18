#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/MeshRenderer.h"

namespace Plaza {
	MeshRenderer::MeshRenderer(Plaza::Mesh* initialMesh, bool addToScene) {
		this->uuid = Plaza::UUID::NewUUID();
		this->mesh = initialMesh;//std::make_unique<Mesh>(initialMesh);
		if (addToScene)
			Application->activeScene->meshRenderers.emplace_back(this);
	}

	MeshRenderer::MeshRenderer(Plaza::Mesh* initialMesh, Material material, bool addToScene) {
		this->uuid = Plaza::UUID::NewUUID();
		this->mesh = initialMesh;
		this->material = &material;

		auto renderGroupIt = Application->activeScene->renderGroupsFindMap.find(std::make_pair(this->mesh->uuid, this->material->uuid));
		if (renderGroupIt != Application->activeScene->renderGroupsFindMap.end()) {
			this->renderGroup = Application->activeScene->renderGroups.at(renderGroupIt->second);
		}
		else {
			uint64_t renderGroupUuid = Plaza::UUID::NewUUID();
			RenderGroup* renderGroup = new RenderGroup(this->mesh, this->material);
			renderGroup->uuid = renderGroupUuid;
			Application->activeScene->AddRenderGroup(renderGroup);
			//Application->activeScene->renderGroups.emplace(renderGroupUuid, new RenderGroup(this->mesh, this->material));
			std::cout << "ok \n";
			uint64_t meshUuid = this->mesh->uuid;
			uint64_t materialUuid = this->material->uuid;
			//Application->activeScene->renderGroupsFindMap.emplace(std::make_pair(meshUuid, materialUuid), renderGroupUuid);
			this->renderGroup = Application->activeScene->renderGroups.at(renderGroupUuid);
		}
		if (addToScene)
			Application->activeScene->meshRenderers.emplace_back(this);

	}

	MeshRenderer::~MeshRenderer() {
		// TODO: FIX MESHRENDERER DELETION
		//if (!Application->activeScene->mIsDeleting) {
		//	if (this->renderGroup)
		//		Application->activeScene->RemoveRenderGroup(this->renderGroup->uuid);
		//	Application->activeScene->RemoveMeshRenderer(this->uuid);
		//}
		//this->renderGroup.~shared_ptr();
	}

	void MeshRenderer::ChangeMaterial(Material* newMaterial) {
		uint64_t oldUuid = newMaterial->uuid;
		this->material = newMaterial;
		this->renderGroup = Application->activeScene->AddRenderGroup(new RenderGroup(this->mesh, this->material));
		//this->renderGroup->ChangeMaterial(newMaterial);
	}
	void MeshRenderer::ChangeMesh(Mesh* newMesh) {

	}
}