#pragma once
#include <imgui/imgui.h>

#include "Engine/Components/Core/Entity.h"
#include "Editor/GUI/guiMain.h"
#include "Editor/GUI/Style/EditorStyle.h"
namespace Plaza::Editor {
	class Gui::Hierarchy {
	public:
		class Item { // Item consists of a Treenode and a selectable
		public:
			Entity& currentObj;
			Entity& selectedGameObject;
			std::string payloadName = "TreeNodeItemPayload";

			Item(Entity& entity, Entity*& selectedGameObject);
			void HierarchyDragDrop(Entity& entity, Entity* currentObj, ImVec2 treeNodeMin, ImVec2 treeNodeMax);

			static void ItemPopup(Entity& entity);
		};
	};
}