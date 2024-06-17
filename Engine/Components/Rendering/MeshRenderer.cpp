#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/MeshRenderer.h"

namespace Plaza {
	MeshRenderer::MeshRenderer(Plaza::Mesh* initialMesh, bool addToScene) {
		this->uuid = Plaza::UUID::NewUUID();
		this->mesh = initialMesh;//std::make_unique<Mesh>(initialMesh);
		if (addToScene)
			Application->activeScene->meshRenderers.emplace_back(this);
	}

	MeshRenderer::MeshRenderer(Plaza::Mesh* initialMesh, std::vector<Material*> materials, bool addToScene) {
		this->uuid = Plaza::UUID::NewUUID();
		this->mesh = initialMesh;
		this->mMaterials = materials;

		auto renderGroupIt = Application->activeScene->renderGroupsFindMap.find(std::make_pair(this->mesh->uuid, Material::GetMaterialsUuids(this->mMaterials)));
		if (renderGroupIt != Application->activeScene->renderGroupsFindMap.end()) {
			this->renderGroup = Application->activeScene->renderGroups.at(renderGroupIt->second);
		}
		else {
			RenderGroup* renderGroup = Application->activeScene->AddRenderGroup(this->mesh, this->mMaterials);//new RenderGroup(this->mesh, this->mMaterials);
			uint64_t renderGroupUuid = renderGroup->uuid;
			//Application->activeScene->AddRenderGroup(renderGroup);
			//Application->activeScene->renderGroups.emplace(renderGroupUuid, new RenderGroup(this->mesh, this->material));

			uint64_t meshUuid = this->mesh->uuid;
			//uint64_t materialUuid = this->material->uuid;
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

	void MeshRenderer::ChangeMaterial(Material* newMaterial, unsigned int index) {
		uint64_t oldUuid = newMaterial->uuid;
		this->mMaterials[index] = newMaterial;
		this->renderGroup = Application->activeScene->AddRenderGroup(new RenderGroup(this->mesh, this->mMaterials));
		//this->renderGroup->ChangeMaterial(newMaterial);
	}
	void MeshRenderer::ChangeMesh(Mesh* newMesh) {

	}
}