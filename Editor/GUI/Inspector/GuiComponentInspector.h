#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Drawing/UI/Gui.h"
#include "Engine/Components/Drawing/UI/GuiButton.h"
#include "Engine/Components/Drawing/UI/GuiRectangle.h"
#include "Editor/GUI/Utils/DataVisualizer.h"
#include "Editor/GUI/Utils/Utils.h"
#include <functional>
#include <iostream>

namespace Plaza::Editor {
	static class GuiComponentInspector {
	public:
		static inline int itemIndex = 0;
		GuiComponentInspector(GuiComponent* gui) {
			if (Utils::ComponentInspectorHeader(gui, "Gui Component Inspector")) {
				itemIndex = 0;
				ImGui::PushID("GuiComponentInspector");

				for (auto& [uuid, item] : gui->mGuiItems) {
					if (gui->mGuiItems.at(uuid)->mGuiParentUuid == 0) {
						GuiItemInspector(gui, item.get());
					}
				}

				itemIndex++;
				ImGui::PushID(itemIndex);
				AddGuiItem(gui, 0);
				ImGui::PopID();

				ImGui::PopID();
			}
		}

		static void AddGuiItem(GuiComponent* gui, uint64_t parentUuid) {
			if (ImGui::Button("Add Item")) {
				ImGui::OpenPopup("AddGuiItemContext");
			}

			if (ImGui::BeginPopupContextWindow("AddGuiItemContext")) {
				if (ImGui::IsMouseReleased(1)) // Check if right mouse button was released
				{
					ImGui::CloseCurrentPopup(); // Close the context menu on right-click
				}

				if (ImGui::MenuItem("Rectangle"))
				{
					std::shared_ptr<GuiRectangle> rectangle = std::make_shared<GuiRectangle>("Rectangle", 0, 0, 100, 100, 1.0f, glm::vec4(1.0f));
					rectangle->mGuiParentUuid = parentUuid;
					gui->NewGuiItem<GuiRectangle>(rectangle);
				}
				else if (ImGui::MenuItem("Button"))
				{
					std::shared_ptr<GuiButton> button = std::make_shared<GuiButton>("Button", "Click", 0, 0, 100, 100, 1.0f, 1.0f, glm::vec4(1.0f));
					button->mGuiParentUuid = parentUuid;
					gui->NewGuiItem<GuiButton>(button);
				}

				ImGui::EndPopup();
			}
		}

		static void GuiItemInspector(GuiComponent* gui, GuiItem* item) {
			itemIndex++;
			ImGui::PushID(itemIndex);

			ImGui::Text(std::string("Name: " + item->mGuiName).c_str());
			AddGuiItem(gui, item->mGuiUuid);

			ImGui::PopID();
			for (uint64_t uuid : item->mGuiChildren) {
				GuiItemInspector(gui, gui->GetGuiItem<GuiItem>(uuid).get());
			}
		}
	};
}