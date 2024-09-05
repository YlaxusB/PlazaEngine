#pragma once
#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Rendering/Material.h"
#include "Engine/Components/Core/Entity.h"
#include "Editor/GUI/Utils/Utils.h"
#include "Editor/GUI/Inspector/FileInspector/MaterialFileInspector.h"
#include "Editor/GUI/ContextWindows/SearchContext.h"

namespace Plaza::Editor {
	static class MeshRendererInspector {
	public:
		static inline SearchContext* sMaterialsSearch = new SearchContext("MaterialSearchContext");

		MeshRendererInspector(Entity* entity) {
			if (Utils::ComponentInspectorHeader(entity->GetComponent<MeshRenderer>(), "Mesh Renderer")) {
				ImGui::PushID("MeshRendererInspector");
				MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();

				if (ImGui::TreeNodeEx("Materials List", ImGuiTreeNodeFlags_DefaultOpen)) {
					unsigned int index = 0;
					for (Material* material : meshRenderer->mMaterials) {
						ImGui::PushID(material->uuid);
						std::string treeNodeName = std::to_string(index) + ": " + material->name;
						index++;
						bool treeNodeOpen = ImGui::TreeNode(treeNodeName.c_str());
						ImGui::SameLine();
						if (ImGui::Button("Change Material")) {
							sMaterialsSearch->SetOpen(true);
							sMaterialsSearch->mCallback = [&](uint64_t uuid) {
								meshRenderer->ChangeMaterial(Application->activeScene->materials.at(uuid).get());
								};
						}

						if (treeNodeOpen) {
							if (AssetsManager::GetAsset(material->mAssetUuid) == nullptr) {
								ImGui::TreePop();
								ImGui::PopID();
								continue;
							}
							File file = File(material->name, AssetsManager::GetAsset(material->mAssetUuid)->mPath.string(), Standards::materialExtName);
							Plaza::Editor::MaterialFileInspector::MaterialFileInspector(&file);

							ImGui::TreePop();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}

				if (sMaterialsSearch->IsOpen())
					sMaterialsSearch->Update();
				ImGui::PopID();
			}

		}
	};
}