#pragma once
#include <imgui/imgui.h>

#include "Engine/Components/Core/GameObject.h"
#include "Editor/GUI/guiMain.h"
#include "Editor/GUI/Style/EditorStyle.h"
namespace Engine::Editor {
	class Gui::Hierarchy {
	public:
		class Item { // Item consists of a Treenode and a selectable
		public:
			GameObject*& currentObj;
			GameObject*& selectedGameObject;
			std::string payloadName = "TreeNodeItemPayload";

			Item(GameObject* gameObject, GameObject*& selectedGameObject);
			void HierarchyDragDrop(GameObject* gameObject, GameObject* currentObj, ImVec2 treeNodeMin, ImVec2 treeNodeMax);
		};
	};
}