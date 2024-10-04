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
					rectangle->mComponentUuid = gui->mUuid;
					rectangle->mGuiParentUuid = parentUuid;
					rectangle->mGuiType = GuiType::PL_GUI_RECTANGLE;
					gui->NewGuiItem<GuiRectangle>(rectangle);
				}
				else if (ImGui::MenuItem("Button"))
				{
					std::shared_ptr<GuiButton> button = std::make_shared<GuiButton>("Button", "Click", 0, 0, 100, 100, 1.0f, 1.0f, glm::vec4(1.0f));
					button->mComponentUuid = gui->mUuid;
					button->mGuiParentUuid = parentUuid;
					button->mGuiType = GuiType::PL_GUI_BUTTON;
					gui->NewGuiItem<GuiButton>(button);
				}
				//else if (ImGui::MenuItem("Text"))
				//{
				//	std::shared_ptr<GuiButton> text = std::make_shared<GuiText>("Text", "Click", 0, 0, 100, 100, 1.0f, 1.0f, glm::vec4(1.0f));
				//	text->mGuiParentUuid = parentUuid;
				//	text->mGuiType = GuiType::PL_GUI_BUTTON;
				//	gui->NewGuiItem<GuiButton>(text);
				//}

				ImGui::EndPopup();
			}
		}

		static void GuiItemInspector(GuiComponent* gui, GuiItem* item) {
			itemIndex++;
			ImGui::PushID(itemIndex);

			ImGui::InputText("Item Name:", item->mGuiName.data(), item->mGuiName.length() + 256);
			glm::vec2 pos = item->GetLocalPosition();
			if (ImGui::DragFloat2("Position", &pos.x))
				item->SetPosition(pos);

			glm::vec2 size = item->GetLocalSize();
			if (ImGui::DragFloat2("Size", &size.x))
				item->SetSize(size);

			switch (item->mGuiType) {
			case GuiType::PL_GUI_BUTTON:
				char buf[512];
				strcpy_s(buf, static_cast<GuiButton*>(item)->mText.c_str());
				if (ImGui::InputTextEx("Text", "Text", buf, 512, ImVec2(0, 0), ImGuiInputTextFlags_EnterReturnsTrue))
					static_cast<GuiButton*>(item)->mText = buf;
				//ImGui::InputText("Text", buffer, sizeof(buffer));
				ImGui::DragFloat("Font Scale", &static_cast<GuiButton*>(item)->mTextScale);
				break;
			case GuiType::PL_GUI_TEXT:
				//ImGui::InputText("Text", static_cast<GuiButton*>(item)->mText.data(), static_cast<GuiButton*>(item)->mText.length() + 256);
				break;
			}

			AddGuiItem(gui, item->mGuiUuid);

			ImGui::PopID();
			for (uint64_t uuid : item->mGuiChildren) {
				GuiItemInspector(gui, gui->GetGuiItem<GuiItem>(uuid));
			}
		}
	};
}