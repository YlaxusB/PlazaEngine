#pragma once
#include "Engine/Components/Core/GameObject.h"
#include "Engine/GUI/guiMain.h"
#include <imgui/imgui.h>

namespace Gui {
	namespace Hierarchy {
		class Item { // Item consists of a Treenode and a selectable
		public:
			GameObject*& currentObj;
			GameObject*& selectedGameObject;
			std::string payloadName;

			Item(GameObject* gameObject, GameObject*& selectedGameObject) : currentObj(gameObject), selectedGameObject(selectedGameObject) {
				// Push the gameObject id, to prevent it to collpases all the treenodes with same id
				ImGui::PushID(gameObject->id);
				// Start the treenode before the component selectable, but only assign its values after creating the button


				bool treeNodeOpen = false;
				if (gameObject->children.size() > 0) {
					treeNodeOpen = ImGui::TreeNodeEx(gameObject->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick);
				}
				else {
					//treeNodeOpen = ImGui::TreeNodeEx(gameObject->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick);
					ImGui::Selectable(gameObject->name.c_str());
				}



				//ImGui::Selectable(gameObject->name.c_str());
				// Change the selected gameobject if user clicked on the selectable
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					selectedGameObject = gameObject;



				if (gameObject->parent) {
					if (ImGui::BeginDragDropSource()) {
						ImGui::SetDragDropPayload("MyPayload", &gameObject, sizeof(GameObject*));
						ImGui::Text("Drag me!");
						ImGui::EndDragDropSource();
					}
				}

				if (gameObject->parent && ImGui::BeginDragDropTarget()) {
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MyPayload")) {
						if (payload->DataSize == sizeof(GameObject*)) {
							/* The current "object" is not the one that is being hold, but the one that the mouse was over when dropped*/
							GameObject* payloadObj = *static_cast<GameObject**>(payload->Data);  // Dereference the pointer			
							std::vector<GameObject*>& currentChildren = currentObj->parent->children;
							std::vector<GameObject*>& payloadChildren = payloadObj->parent->children;

							// Find the indexes and check if they are valid
							size_t currentIndex = std::distance(currentChildren.begin(), std::find(currentChildren.begin(), currentChildren.end(), currentObj));
							if (currentIndex < currentChildren.size()) {

								size_t payloadIndex = std::distance(payloadChildren.begin(), std::find(payloadChildren.begin(), payloadChildren.end(), payloadObj));
								if (payloadIndex < payloadChildren.size()) {

									// Swap the positions in the children vector of each parent
									std::iter_swap(payloadChildren.begin() + payloadIndex, currentChildren.begin() + currentIndex);

									// Change the parent of the objects
									GameObject* oldPayLoadParent = payloadObj->parent;
									payloadObj->parent = currentObj->parent;
									currentObj->parent = oldPayLoadParent;

									// Update their position in relation to their new parents
									currentObj->transform->relativePosition = currentObj->transform->worldPosition - currentObj->parent->transform->worldPosition;
									payloadObj->transform->relativePosition = payloadObj->transform->worldPosition - payloadObj->parent->transform->worldPosition;

									// Update their children positions
									currentObj->transform->UpdateChildrenTransform();
									payloadObj->transform->UpdateChildrenTransform();
								}
							}
						}
					}
					ImGui::EndDragDropTarget();
				}


				if (treeNodeOpen)
				{
					for (GameObject* child : gameObject->children)
					{
						Gui::Hierarchy::Item(child, selectedGameObject);
					}
					ImGui::TreePop();
				}

				ImGui::PopID();
			}
		};



	}
}
/*
if (ImGui::BeginDragDropTarget()) {
	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MyPayload")) {
		if (payload->DataSize == sizeof(GameObject*)) {
			std::cout << "1" << std::endl;

			GameObject* currentObj = gameObject;
			GameObject* payloadObj = *static_cast<GameObject**>(payload->Data);  // Dereference the pointer			
			std::vector<GameObject*>& children = currentObj->parent->children;
			std::cout << "2" << std::endl;
	
			// Find the iterator of the GameObject with the specified name

			size_t currentIndex = std::distance(children.begin(), std::find(children.begin(), children.end(), gameObject));
			std::cout << "3" << std::endl;
			if (currentIndex < children.size()) {
				// Find the current object, erase it and insert the payloadObj on that position
				auto currentIterator = std::find(children.begin(), children.end(), currentObj);
				children.erase(currentIterator);
				//currentIndex = std::distance(children.begin(), std::find(children.begin(), children.end(), gameObject));
				children.insert(children.begin() + currentIndex, payloadObj);
				std::cout << "4" << std::endl;
				// Find the payload object, erase it and insert the currentObj on that position

				size_t payloadIndex = std::distance(children.begin(), std::find(children.begin(), children.end(), payloadObj));
				if (payloadIndex < children.size()) {
					std::cout << "5" << std::endl;
					auto payloadIterator = std::find(children.begin(), children.end(), payloadObj);
					std::cout << "6" << std::endl;
					children.erase(payloadIterator);
					std::cout << "7" << std::endl;
					//payloadIndex = std::distance(children.begin(), std::find(children.begin(), children.end(), payloadObj));
					children.insert(children.begin() + payloadIndex, currentObj);
					std::cout << "8" << std::endl;

				}

			}


		}
	}
	ImGui::EndDragDropTarget();
}*/