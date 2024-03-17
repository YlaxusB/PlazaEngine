#include "Engine/Core/PreCompiledHeaders.h"
#include "RenderGroup.h"

namespace Plaza {
	void RenderGroup::InitializeInstanceBuffer() {
		VulkanRenderer::GetRenderer()->CreateBuffer(2 * sizeof(glm::mat4), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, mInstanceBuffer, mInstanceBufferMemory);
	}

	void RenderGroup::ChangeMaterial(Material* newMaterial) {
		//Application->activeScene->RemoveRenderGroup(this->uuid);
		uint64_t oldMeshUuid = this->mesh->uuid;
		uint64_t oldMaterialUuid = this->material->uuid;
		this->material = newMaterial;
		Application->activeScene->AddRenderGroup(this);
		RenderGroup* renderGroup = Application->activeScene->GetRenderGroupWithUuids(oldMeshUuid, oldMaterialUuid);
		if (renderGroup) {
			//Application->activeScene->RemoveRenderGroup(renderGroup->uuid);
		}
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