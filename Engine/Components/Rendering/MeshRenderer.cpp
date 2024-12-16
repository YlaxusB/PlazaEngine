#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/MeshRenderer.h"

namespace Plaza {
	MeshRenderer::MeshRenderer(Plaza::Mesh* initialMesh, bool addToScene) {
		this->mUuid = Plaza::UUID::NewUUID();
		this->mesh = initialMesh;//std::make_unique<Mesh>(initialMesh);
		if (addToScene)
			Scene::GetActiveScene()->meshRenderers.emplace_back(this);
	}

	MeshRenderer::MeshRenderer(Plaza::Mesh* initialMesh, std::vector<Material*> materials, bool addToScene) {
		this->mUuid = Plaza::UUID::NewUUID();
		this->mesh = initialMesh;
		this->mMaterials = materials;

		auto renderGroupIt = Scene::GetActiveScene()->renderGroupsFindMap.find(std::make_pair(this->mesh->uuid, Material::GetMaterialsUuids(this->mMaterials)));
		if (renderGroupIt != Scene::GetActiveScene()->renderGroupsFindMap.end()) {
			this->renderGroup = &Scene::GetActiveScene()->renderGroups.at(renderGroupIt->second);
		}
		else {
			RenderGroup* renderGroup = Scene::GetActiveScene()->AddRenderGroup(this->mesh, this->mMaterials);//new RenderGroup(this->mesh, this->mMaterials);
			uint64_t renderGroupUuid = renderGroup->uuid;
			//Scene::GetActiveScene()->AddRenderGroup(renderGroup);
			//Scene::GetActiveScene()->renderGroups.emplace(renderGroupUuid, new RenderGroup(this->mesh, this->material));

			uint64_t meshUuid = this->mesh->uuid;
			//uint64_t materialUuid = this->material->uuid;
			//Scene::GetActiveScene()->renderGroupsFindMap.emplace(std::make_pair(meshUuid, materialUuid), renderGroupUuid);
			this->renderGroup = &Scene::GetActiveScene()->renderGroups.at(renderGroupUuid);
		}
		if (addToScene)
			Scene::GetActiveScene()->meshRenderers.emplace_back(this);

	}

	MeshRenderer::~MeshRenderer() {
		// TODO: FIX MESHRENDERER DELETION
		//if (!Scene::GetActiveScene()->mIsDeleting) {
		//	if (this->renderGroup)
		//		Scene::GetActiveScene()->RemoveRenderGroup(this->renderGroup->uuid);
		//	Scene::GetActiveScene()->RemoveMeshRenderer(this->uuid);
		//}
		//this->renderGroup.~shared_ptr();
	}

	void MeshRenderer::AddMaterial(Material* newMaterial) {
		this->mMaterials.push_back(newMaterial);
		this->renderGroup = Scene::GetActiveScene()->AddRenderGroup(new RenderGroup(this->mesh, this->mMaterials));
		//this->renderGroup->ChangeMaterial(newMaterial);
	}
	void MeshRenderer::ChangeMaterial(Material* newMaterial, unsigned int index) {
		this->mMaterials[index] = newMaterial;
		this->renderGroup = Scene::GetActiveScene()->AddRenderGroup(new RenderGroup(this->mesh, this->mMaterials));
		//this->renderGroup->ChangeMaterial(newMaterial);
	}
	void MeshRenderer::ChangeMesh(Mesh* newMesh) {
		this->mesh = newMesh;
		this->renderGroup = Scene::GetActiveScene()->AddRenderGroup(new RenderGroup(newMesh, this->mMaterials));
	}
}