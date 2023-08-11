#pragma once
#include "Engine/Core/PreCompiledHeaders.h"

#include "Engine/Components/Rendering/Material.h"
#include "Engine/Components/Core/GameObject.h"
#include "Editor/GUI/Inspector.h"
namespace Engine::Editor {
	static class MaterialInspector {
	public:
		MaterialInspector(GameObject* gameObject) {
			if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::PushID("MaterialInspector");
				Material& material = gameObject->GetComponent<MeshRenderer>()->mesh->material;
				glm::vec4& diffuse = material.diffuse.rgba;
				glm::vec4& specular = material.specular.rgba;
				float shininess = material.shininess;

				ImGui::SetNextItemWidth(225);
				ImGui::SliderFloat3("Diffuse", &diffuse.x, 0, 1.0f);
				

				ImGui::SetNextItemWidth(225);
				ImGui::SliderFloat3("Specular", &specular.x, 0, 1.0f);

				ImGui::SetNextItemWidth(225);
				ImGui::SliderFloat("Shininess", &shininess, 0.0f, 100.0f);

				ImGui::Text("Diffuse Path: ");
				ImGui::SameLine();
				ImGui::Text(material.diffuse.path.c_str());

				material.shininess = shininess;

				ImGui::Text("Active Scene: ");
				ImGui::Text(Editor::selectedGameObject->transform->scene.c_str());

				ImGui::PopID();
				ImGui::TreePop();
			}

		}
	};
}