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

				MeshRenderer* meshR = entity->GetComponent<MeshRenderer>();
				RenderGroup* oldRenderGroup = entity->GetComponent<MeshRenderer>()->renderGroup.get();

				for (auto [key, value] : Application->activeScene->materials) {
					if (value.get()->name.empty())
						continue;

					if (ImGui::Button(value->name.c_str())) {
						entity->GetComponent<MeshRenderer>()->material = value.get();
						entity->GetComponent<MeshRenderer>()->renderGroup->mesh = entity->GetComponent<MeshRenderer>()->mesh;
						entity->GetComponent<MeshRenderer>()->renderGroup->ChangeMaterial(value.get());
					}
				}
				RenderGroup* newRenderGroup = entity->GetComponent<MeshRenderer>()->renderGroup.get();

				Material& material = *(entity->GetComponent<MeshRenderer>()->material);
				if (meshRenderer->mesh && meshRenderer->renderGroup.get() && meshRenderer->material) {
					glm::vec4& diffuse = material.diffuse->rgba;
					glm::vec4& specular = material.specular->rgba;
					float shininess = material.shininess;

					//  ImGui::Text("Mesh VAO: ");
					//  ImGui::SameLine();
					//  ImGui::Text(std::to_string(meshRenderer->mesh->VAO).c_str());

					//ImGui::Text("Render Group Mesh VAO: ");
					//ImGui::SameLine();
					//ImGui::Text(std::to_string(meshRenderer->renderGroup->mesh->VAO).c_str());

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
					ImGui::Text(material.diffuse->path.c_str());

					material.shininess = shininess;

					ImGui::Text("Active Scene: ");
					ImGui::Text(Editor::selectedGameObject->GetComponent<Transform>()->scene.c_str());

					ImGui::Checkbox("Cast Shadows", &entity->GetComponent<MeshRenderer>()->castShadows);
					ImGui::DragFloat("Intensity", &entity->GetComponent<MeshRenderer>()->material->intensity);

					ImGui::Text("Max Vector: ");
					ImGui::Text("X: "); ImGui::SameLine(); ImGui::Text(std::to_string(entity->GetComponent<MeshRenderer>()->renderGroup->mesh->mBoundingBox.maxVector.x).c_str());
					ImGui::Text("Y: "); ImGui::SameLine(); ImGui::Text(std::to_string(entity->GetComponent<MeshRenderer>()->renderGroup->mesh->mBoundingBox.maxVector.y).c_str());
					ImGui::Text("Z: "); ImGui::SameLine(); ImGui::Text(std::to_string(entity->GetComponent<MeshRenderer>()->renderGroup->mesh->mBoundingBox.maxVector.z).c_str());

					ImGui::Text("Min Vector: ");
					ImGui::Text("X: "); ImGui::SameLine(); ImGui::Text(std::to_string(entity->GetComponent<MeshRenderer>()->renderGroup->mesh->mBoundingBox.minVector.x).c_str());
					ImGui::Text("Y: "); ImGui::SameLine(); ImGui::Text(std::to_string(entity->GetComponent<MeshRenderer>()->renderGroup->mesh->mBoundingBox.minVector.y).c_str());
					ImGui::Text("Z: "); ImGui::SameLine(); ImGui::Text(std::to_string(entity->GetComponent<MeshRenderer>()->renderGroup->mesh->mBoundingBox.minVector.z).c_str());

				}
				ImGui::PopID();
			}

		}
	};
}