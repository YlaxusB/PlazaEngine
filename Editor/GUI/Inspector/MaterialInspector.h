#pragma once
#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Rendering/Material.h"
#include "Engine/Components/Core/Entity.h"
#include "Editor/GUI/Utils/Utils.h"

namespace Plaza::Editor {
	static class MaterialInspector {
	public:
		MaterialInspector(Entity* entity) {
			if (Utils::ComponentInspectorHeader(entity->GetComponent<MeshRenderer>(), "Material")) {
				ImGui::PushID("MaterialInspector");
				MeshRenderer* meshRenderer = entity->GetComponent<MeshRenderer>();
				Material& material = *(entity->GetComponent<MeshRenderer>()->material);
				glm::vec4& diffuse = material.diffuse.rgba;
				glm::vec4& specular = material.specular.rgba;
				float shininess = material.shininess;

				if (meshRenderer->mesh.get() && meshRenderer->renderGroup.get()) {
					ImGui::Text("Mesh VAO: ");
					ImGui::SameLine();
					ImGui::Text(std::to_string(meshRenderer->mesh->VAO).c_str());

					ImGui::Text("Render Group Mesh VAO: ");
					ImGui::SameLine();
					ImGui::Text(std::to_string(meshRenderer->renderGroup->mesh->VAO).c_str());

					ImGui::Text("Material Name: ");
					ImGui::SameLine();
					ImGui::Text(material.name.c_str());

					ImGui::Text("Render Group Material Name: ");
					ImGui::SameLine();
					ImGui::Text(entity->GetComponent<MeshRenderer>()->renderGroup->material->name.c_str());

					ImGui::Text("Material Path: ");
					ImGui::SameLine();
					ImGui::Text(material.filePath.c_str());

					ImGui::Text("Material Uuid: ");
					ImGui::SameLine();
					ImGui::Text(std::to_string(material.uuid).c_str());

					ImGui::SetNextItemWidth(225);
					ImGui::SliderFloat4("Diffuse", &diffuse.x, 0, 1.0f);


					ImGui::SetNextItemWidth(225);
					ImGui::SliderFloat4("Specular", &specular.x, 0, 1.0f);

					ImGui::SetNextItemWidth(225);
					ImGui::DragFloat("Shininess", &shininess, 0.01f, 0.0f, 10.0f);

					ImGui::Text("Diffuse Path: ");
					ImGui::SameLine();
					ImGui::Text(material.diffuse.path.c_str());

					material.shininess = shininess;

					ImGui::Text("Active Scene: ");
					ImGui::Text(Editor::selectedGameObject->GetComponent<Transform>()->scene.c_str());

					ImGui::Checkbox("Cast Shadows", &entity->GetComponent<MeshRenderer>()->castShadows);

					MeshRenderer* meshR = entity->GetComponent<MeshRenderer>();
					RenderGroup* oldRenderGroup = entity->GetComponent<MeshRenderer>()->renderGroup.get();
					for (auto [key, value] : Application->activeScene->materials) {
						if (!value.get()->name.empty() && ImGui::Button(value->name.c_str())) {
							entity->GetComponent<MeshRenderer>()->material = value;
							entity->GetComponent<MeshRenderer>()->renderGroup->material = value;
						}
					}
					RenderGroup* newRenderGroup = entity->GetComponent<MeshRenderer>()->renderGroup.get();

					if (ImGui::Button("New Material")) {
						uint64_t renderGroupUuid = Plaza::UUID::NewUUID();
						std::shared_ptr<Material> material = std::make_shared<Material>(Material());
						RenderGroup* renderGroup = new RenderGroup(entity->GetComponent<MeshRenderer>()->mesh, material);
						renderGroup->uuid = renderGroupUuid;
						Application->activeScene->AddRenderGroup(renderGroup);
						entity->GetComponent<MeshRenderer>()->renderGroup = Application->activeScene->renderGroups.at(renderGroupUuid);
						/*
								auto renderGroupIt = Application->activeScene->renderGroupsFindMap.find(std::make_pair(this->mesh->uuid, this->material->uuid));
								if (renderGroupIt != Application->activeScene->renderGroupsFindMap.end()) {
									this->renderGroup = Application->activeScene->renderGroups.at(renderGroupIt->second);
								}
								else {
									uint64_t renderGroupUuid = Plaza::UUID::NewUUID();
									Application->activeScene->renderGroups.emplace(renderGroupUuid, new RenderGroup(this->mesh, this->material));
									Application->activeScene->renderGroupsFindMap.emplace(std::make_pair(this->mesh->uuid, this->material->uuid), renderGroupUuid);
									this->renderGroup = Application->activeScene->renderGroups.at(renderGroupUuid);
								}*/
					}
				}
				ImGui::PopID();
			}

		}
	};
}