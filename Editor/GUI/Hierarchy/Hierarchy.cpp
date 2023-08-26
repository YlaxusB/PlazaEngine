#include "Engine/Core/PreCompiledHeaders.h"
#include "Editor/GUI/Hierarchy/Hierarchy.h"
#include "Engine/Editor/Editor.h"
#include "Editor/GUI/guiMain.h"
#include "Editor/GUI/Hierarchy/HierarchyPopup.h"
#include "Editor/DefaultAssets/DefaultAssets.h"

#include "Editor/GUI/Popups/NewEntityPopup.h"
namespace Plaza::Editor {
	Gui::Hierarchy::Item::Item(Entity& entity, Entity*& selectedGameObject) : currentObj(entity), selectedGameObject(*selectedGameObject) {
		// Push the entity id, to prevent it to collpases all the treenodes with same id
		ImGui::PushID(entity.uuid);
		// Start the treenode before the component selectable, but only assign its values after creating the button

		ImGui::PushStyleColor(ImGuiCol_HeaderActive, editorStyle.treeNodeActiveBackgroundColor);

		bool itemIsSelectedObject = false;
		if (selectedGameObject && entity.uuid == selectedGameObject->uuid) itemIsSelectedObject = true;

		if (itemIsSelectedObject) {// Selected backgroundde
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, editorStyle.selectedTreeNodeBackgroundColor);
			ImGui::PushStyleColor(ImGuiCol_Header, editorStyle.selectedTreeNodeBackgroundColor);
		}
		else {// Not selected background
			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, editorStyle.treeNodeHoverBackgroundColor);
			ImGui::PushStyleColor(ImGuiCol_Header, editorStyle.treeNodeBackgroundColor);
		}



		bool treeNodeOpen = false;

		ImGuiStyle& style = ImGui::GetStyle();

		style.IndentSpacing = 3.0f;
		//ImGui::SetCursorPosX(ImGui::GetCursorPosX());
		const float indentSpacing = ImGui::GetStyle().IndentSpacing;
		const int depth = 1.0f / indentSpacing;

		ImGui::Indent(3.0f);
		//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetTreeNodeToLabelSpacing() + 3.0f); // Decrease the indentation spacing
		if (entity.childrenUuid.size() > 0) {
			treeNodeOpen = ImGui::TreeNodeEx(entity.name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen);
		}
		else {

			//ImGui::Selectable(entity->name.c_str(), ImGuiTreeNodeFlags_Framed);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetTreeNodeToLabelSpacing());
			treeNodeOpen = ImGui::TreeNodeEx(entity.name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Leaf);
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

		// Change the selected entity if user clicked on the selectable
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			Gui::changeSelectedGameObject(&Application->activeScene->entities[entity.uuid]);
		//Plaza::Editor::selectedGameObject = entity;


		if (ImGui::IsItemVisible()) {
			if (entity.parentUuid && ImGui::BeginDragDropSource()) {
				ImGui::SetDragDropPayload(payloadName.c_str(), &entity, sizeof(Entity*));

				ImVec2 mousePos = ImGui::GetMousePos();
				ImVec2 treeNodePos = treeNodeMin;
				ImVec2 treeNodeSize = ImVec2(treeNodeMax.x - treeNodeMin.x, treeNodeMax.y - treeNodeMin.y);

				ImVec2 treeNodeCenter = ImVec2(treeNodePos.x, treeNodePos.y + treeNodeSize.y / 2);
				ImGui::EndDragDropSource();
			}

			Hierarchy::Item::HierarchyDragDrop(entity, &currentObj, treeNodeMin, treeNodeMax);
		}
		if (ImGui::IsItemHovered() || ImGui::IsPopupOpen("ItemPopup")) {
			Gui::Hierarchy::Item::ItemPopup(entity);
		}

		if (ImGui::IsPopupOpen("ItemPopup")) {
			Gui::changeSelectedGameObject(&Application->activeScene->entities[entity.uuid]);
		}

		if (treeNodeOpen)
		{
			for (uint64_t child : entity.childrenUuid)
			{
				Gui::Hierarchy::Item(Application->activeScene->entities[child], selectedGameObject);
			}
			ImGui::TreePop();
		}
		ImGui::Unindent(indentSpacing * depth);


		ImGui::PopID();
	};

	void Gui::Hierarchy::Item::ItemPopup(Entity& entity) {
		if (ImGui::BeginPopupContextWindow("ItemPopup"))
		{
			if (ImGui::BeginMenu("Add Component"))
			{
				if (ImGui::MenuItem("Mesh Renderer"))
				{
					MeshRenderer* meshRenderer = new MeshRenderer(*Editor::DefaultModels::Cube());
					meshRenderer->instanced = true;
					meshRenderer->mesh = DefaultModels::Cube();
					entity.AddComponent<MeshRenderer>(meshRenderer);
				}

				if (ImGui::MenuItem("Rigid Body Dynamic"))
				{
					RigidBody* rigidBody = new RigidBody(entity.uuid, Application->runningScene);
					rigidBody->uuid = entity.uuid;
					//Collider* collider = new Collider(entity.uuid);
					//entity.AddComponent<Collider>(collider);
					//entity.GetComponent<Collider>()->Init();
					entity.AddComponent<RigidBody>(rigidBody);
				}

				if (ImGui::MenuItem("Rigid Body Non Dynamic"))
				{
					RigidBody* rigidBody = new RigidBody(entity.uuid, Application->runningScene, false);
					rigidBody->dynamic = false;
					rigidBody->uuid = entity.uuid;
					entity.AddComponent<RigidBody>(rigidBody);
				}

				if (ImGui::MenuItem("Collider"))
				{
					Collider* collider = new Collider(entity.uuid);
					entity.AddComponent<Collider>(collider);
				}

				if (ImGui::MenuItem("Camera"))
				{
					Camera* camera = new Camera();
					camera->uuid = entity.uuid;
					entity.AddComponent<Camera>(camera);
				}

				ImGui::EndMenu();
			}

			Popup::NewEntityPopup::Init(&entity, &entity);

			ImGui::EndPopup();
		}
	}
};
