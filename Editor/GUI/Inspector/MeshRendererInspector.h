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
						std::string idString = std::to_string(material->mAssetUuid) + "meshRendererMaterial";
						ImGui::PushID(idString.c_str());
						std::string treeNodeName = std::to_string(index) + ": " + material->mAssetName;
						bool treeNodeOpen = ImGui::TreeNode(treeNodeName.c_str());
						ImGui::SameLine();
						if (ImGui::Button("Change Material")) {
							sMaterialsSearch->SetOpen(true);
							sMaterialsSearch->mCallback = [&, index](uint64_t uuid) {
								meshRenderer->ChangeMaterial(AssetsManager::mMaterials.at(uuid).get(), index);
								};
						}
						ImGui::SameLine();
						if (ImGui::Button("Remove Button")) {
							meshRenderer->mMaterials.erase(meshRenderer->mMaterials.begin() + index);
						}

						index++;
						if (treeNodeOpen) {
							if (AssetsManager::GetAsset(material->mAssetUuid) == nullptr) {
								ImGui::TreePop();
								ImGui::PopID();
								continue;
							}
							File file = File(material->mAssetName, AssetsManager::GetAsset(material->mAssetUuid)->mAssetPath.string(), Standards::materialExtName);
							Plaza::Editor::MaterialFileInspector::MaterialFileInspector(&file);

							ImGui::TreePop();
						}
						ImGui::PopID();
					}

					if (ImGui::Button("Add Material")) {
						sMaterialsSearch->SetOpen(true);
						sMaterialsSearch->mCallback = [&](uint64_t uuid) {
							meshRenderer->mMaterials.push_back(AssetsManager::mMaterials.at(uuid).get());
							};
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