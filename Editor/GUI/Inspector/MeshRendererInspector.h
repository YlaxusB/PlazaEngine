#pragma once
#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Rendering/Material.h"
#include "Engine/Components/Core/Entity.h"
#include "Editor/GUI/Utils/Utils.h"
#include "Editor/GUI/Inspector/FileInspector/MaterialFileInspector.h"

namespace Plaza::Editor {
	static class MeshRendererInspector {
	public:
		MeshRendererInspector(Entity* entity) {
			if (Utils::ComponentInspectorHeader(entity->GetComponent<MeshRenderer>(), "Mesh Renderer")) {
				ImGui::PushID("MeshRendererInspector");
				MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();

				MeshRenderer* meshR = entity->GetComponent<MeshRenderer>();
				RenderGroup* oldRenderGroup = entity->GetComponent<MeshRenderer>()->renderGroup;

				if (oldRenderGroup) {
					for (unsigned int i = 0; i < meshRenderer->mMaterials.size(); ++i) {
						ImGui::Text("MeshRenderer Material: ");
						ImGui::SameLine();
						ImGui::Text(meshRenderer->mMaterials[i]->name.c_str());
						ImGui::Text("RenderGroup Material: ");
						ImGui::SameLine();
						ImGui::Text(meshRenderer->renderGroup->materials[i]->name.c_str());

						ImGui::Text("Diffuse Handle: ");
						ImGui::SameLine();
						ImGui::Text(std::to_string(meshRenderer->renderGroup->materials[i]->diffuse->mIndexHandle).c_str());

						ImGui::Text("Material Handle: ");
						ImGui::SameLine();
						ImGui::Text(std::to_string(meshRenderer->renderGroup->materials[i]->mIndexHandle).c_str());

						for (auto [key, value] : Application->activeScene->materials) {
							Asset* asset = AssetsManager::GetAsset(value->uuid);
							if (!asset || value->uuid == 0)
								continue;
							std::string name = asset->mPath.stem().string();
							if (value->uuid == 0)
								name = value->name;


							if (ImGui::Button(asset->mPath.stem().string().c_str())) {
								entity->GetComponent<MeshRenderer>()->ChangeMaterial(value.get());
							}
						}
					}

					for (Material* material : meshR->mMaterials) {
						if (AssetsManager::GetAsset(material->mAssetUuid) == nullptr)
							continue;
						File* file = new File(material->name, AssetsManager::GetAsset(material->mAssetUuid)->mPath.string(), Standards::materialExtName);
						Plaza::Editor::MaterialFileInspector::MaterialFileInspector(file);
						delete(file);
					}
				}
				ImGui::PopID();
			}

		}
	};
}