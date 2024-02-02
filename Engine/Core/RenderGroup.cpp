#include "Engine/Core/PreCompiledHeaders.h"
#include "RenderGroup.h"

namespace Plaza {
	void RenderGroup::ChangeMaterial(Material* newMaterial) {
		//Application->activeScene->RemoveRenderGroup(this->uuid);
		this->material = newMaterial;
		Application->activeScene->AddRenderGroup(this);
	}

	void RenderGroup::ChangeMesh(Mesh* mesh) {
		//Application->activeScene->RemoveRenderGroup(this->uuid);
		this->mesh = mesh;
		Application->activeScene->AddRenderGroup(this);
	}

	bool RenderGroup::SceneHasRenderGroup(Mesh* mesh, Material* material) {
		bool hasSpecificRenderGroup = Application->activeScene->renderGroupsFindMap.find(std::pair<uint64_t, uint64_t>(mesh->uuid, material->uuid)) != Application->activeScene->renderGroupsFindMap.end();
		return hasSpecificRenderGroup;
	}

	bool RenderGroup::SceneHasRenderGroup(RenderGroup* renderGroup) {
		return SceneHasRenderGroup(renderGroup->mesh, renderGroup->material);
	}
}