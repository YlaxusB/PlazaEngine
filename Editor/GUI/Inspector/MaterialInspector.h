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
				Material& material = *(entity->GetComponent<MeshRenderer>()->renderGroup->material);
				glm::vec4& diffuse = material.diffuse.rgba;
				glm::vec4& specular = material.specular.rgba;
				float shininess = material.shininess;

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

				if(ImGui::Button("New Material")) {
					uint64_t renderGroupUuid = Plaza::UUID::NewUUID();
					std::shared_ptr<Material> material = std::make_shared<Material>(Material());
					Application->activeScene->renderGroups.emplace(renderGroupUuid, new RenderGroup(entity->GetComponent<MeshRenderer>()->mesh, material));
					Application->activeScene->renderGroupsFindMap.emplace(std::make_pair(entity->GetComponent<MeshRenderer>()->mesh->uuid, material->uuid), renderGroupUuid);
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

				ImGui::PopID();
			}

		}
	};
}