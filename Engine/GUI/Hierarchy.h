#pragma once
#include "Engine/Components/Core/GameObject.h"
#include "Engine/GUI/guiMain.h"
#include <imgui/imgui.h>

namespace Gui {
	namespace Hierarchy {
		class Item { // Item consists of a Treenode and a selectable
		public:
			GameObject* gameObject;
			GameObject*& selectedGameObject;
			std::string payloadName;

			Item(GameObject* gameObject, GameObject*& selectedGameObject) : gameObject(gameObject), selectedGameObject(selectedGameObject) {
				// Push the gameObject id, to prevent it to collpases all the treenodes with same id
				ImGui::PushID(gameObject->id);
				// Start the treenode before the component selectable, but only assign its values after creating the button
				bool treeNodeOpen = false;
				if (gameObject->children.size() > 0) {
					treeNodeOpen = ImGui::TreeNodeEx("");
					ImGui::SameLine();
				}



				ImGui::Selectable(gameObject->name.c_str());
				// Change the selected gameobject if user clicked on the selectable
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					selectedGameObject = gameObject;

				if (treeNodeOpen)
				{
					for (GameObject* child : gameObject->children)
					{
						Gui::Hierarchy::Item(child, selectedGameObject);
					}
					ImGui::TreePop();
				}
				ImGui::PopID();

				if (gameObject) {
					if (ImGui::BeginDragDropSource()) {
						ImGui::SetDragDropPayload("MyPayload", &gameObject, sizeof(GameObject*));
						ImGui::Text("Drag me!");
						ImGui::EndDragDropSource();
					}
				}

				if (ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MyPayload")) {
						if (payload->DataSize == sizeof(GameObject*)) {  // Check the payload data size
							GameObject* payloadObj = *static_cast<GameObject**>(payload->Data);  // Dereference the pointer							
							//std::cout << gameObjects. << std::endl;
							// Handle the dropped data here
						}
					}
					ImGui::EndDragDropTarget();
				}
			}
		};



	}
}
