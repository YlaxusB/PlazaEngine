#include "Engine/GUI/Hierarchy/Hierarchy.h"
#include "Engine/Editor/Editor.h"
namespace Editor {
	namespace Gui {
		namespace Hierarchy {
			Item::Item(GameObject* gameObject, GameObject*& selectedGameObject) : currentObj(gameObject), selectedGameObject(selectedGameObject) {
				// Push the gameObject id, to prevent it to collpases all the treenodes with same id
				ImGui::PushID(gameObject->id);
				// Start the treenode before the component selectable, but only assign its values after creating the button

				ImGui::PushStyleColor(ImGuiCol_HeaderActive, editorStyle.treeNodeActiveBackgroundColor);

				bool itemIsSelectedObject = false;
				if (selectedGameObject) itemIsSelectedObject = gameObject->id == selectedGameObject->id;

				if (itemIsSelectedObject) {// Selected background
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, editorStyle.selectedTreeNodeBackgroundColor);
					ImGui::PushStyleColor(ImGuiCol_Header, editorStyle.selectedTreeNodeBackgroundColor);
				}
				else {// Not selected background
					ImGui::PushStyleColor(ImGuiCol_HeaderHovered, editorStyle.treeNodeHoverBackgroundColor);
					ImGui::PushStyleColor(ImGuiCol_Header, editorStyle.treeNodeBackgroundColor);
				}

				bool treeNodeOpen = false;

				ImGuiStyle& style = ImGui::GetStyle();

				style.IndentSpacing = 11.0f;
				//ImGui::SetCursorPosX(ImGui::GetCursorPosX());
				const float indentSpacing = ImGui::GetStyle().IndentSpacing;
				const int depth = 1.0f / indentSpacing;

				ImGui::Indent(11.0f);
				//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetTreeNodeToLabelSpacing() + 3.0f); // Decrease the indentation spacing
				if (gameObject->children.size() > 0) {
					treeNodeOpen = ImGui::TreeNodeEx(gameObject->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen);
				}
				else {

					//ImGui::Selectable(gameObject->name.c_str(), ImGuiTreeNodeFlags_Framed);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
					treeNodeOpen = ImGui::TreeNodeEx(gameObject->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf);
				}

				//ImGui::SetWindowPos(ImVec2(ImGui::GetWindowPos().x - 1.0f, ImGui::GetWindowPos().y));

				// Get the start and end position of the header
				ImVec2 treeNodeMin = ImGui::GetItemRectMin();
				ImVec2 treeNodeMax = ImGui::GetItemRectMax();
				ImVec2 treeNodePos = ImVec2(treeNodeMin.x, treeNodeMin.y);
				//ImGui::PopStyleVar(); // IndentSpacing

				ImGui::PopStyleColor(); // Header Active Background (Active is when I click on it)
				ImGui::PopStyleColor(); // Header Hovered Background
				ImGui::PopStyleColor(); // Header Selected Background

				// Change the selected gameobject if user clicked on the selectable
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					Engine::Editor::selectedGameObject = gameObject;
				


				if (gameObject->parent) {
					if (ImGui::BeginDragDropSource()) {
						ImGui::SetDragDropPayload(payloadName.c_str(), &gameObject, sizeof(GameObject*));

						ImVec2 mousePos = ImGui::GetMousePos();
						ImVec2 treeNodePos = treeNodeMin;
						ImVec2 treeNodeSize = ImVec2(treeNodeMax.x - treeNodeMin.x, treeNodeMax.y - treeNodeMin.y);

						ImVec2 treeNodeCenter = ImVec2(treeNodePos.x, treeNodePos.y + treeNodeSize.y / 2);

						ImGui::Text("Mouse Pos: x %.1f, y %.1f", mousePos.x, mousePos.y);
						ImGui::Text("Tree Node Pos: x %.1f, y %.1f", treeNodePos.x, treeNodePos.y);
						ImGui::Text("Tree Node Size: x %.1f, y %.1f", treeNodeSize.x, treeNodeSize.y);
						ImGui::Text("Tree Node Center: x %.1f, y %.1f", treeNodeCenter.x, treeNodeCenter.y);
						ImGui::EndDragDropSource();
					}
				}

				Hierarchy::Item::HierarchyDragDrop(gameObject, currentObj, treeNodeMin, treeNodeMax);

				if (treeNodeOpen)
				{
					for (GameObject* child : gameObject->children)
					{
						Gui::Hierarchy::Item(child, selectedGameObject);
					}
					ImGui::TreePop();

				}
				ImGui::Unindent(indentSpacing * depth);

				ImGui::PopID();

			}
		}
	}
}