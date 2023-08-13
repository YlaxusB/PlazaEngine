#include "Engine/Core/PreCompiledHeaders.h"
#include "File.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
namespace Engine {
	namespace Editor {
		std::map<std::string, IconTexture> Icon::textures = std::map<std::string, IconTexture>();

		float Editor::File::iconSize = 75.0f;
		float Editor::File::spacing = 5.0f;
		ImVec2 Editor::File::currentPos = ImVec2(-1.0f, 1.0f);

		void Editor::File::Update() {

			if (currentPos.x == -1.0f) {
				currentPos = ImGui::GetCursorScreenPos();
			}
			ImGui::SetCursorScreenPos(currentPos);

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoFocusOnAppearing;
			windowFlags |= ImGuiWindowFlags_NoScrollbar;

			if (ImGui::BeginChild(ImGui::GetID(this->name.c_str()), ImVec2(75, 75), false, windowFlags)) {
				ImVec4 backgroundColor = editorStyle.hierarchyBackgroundColor;

				if (ImGui::IsWindowHovered()) {
					backgroundColor = editorStyle.treeNodeHoverBackgroundColor;
				}

				if (ImGui::BeginDragDropSource()) {
					File* filePtr = new File(*this);
					ImGui::Text(filePtr->name.c_str());
					ImGui::Text(filePtr->directory.c_str());
					ImGui::SetDragDropPayload(Gui::scenePayloadName.c_str(), &filePtr, sizeof(Editor::File&));
					ImGui::EndDragDropSource();
				}
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 size = ImGui::imVec2(ImGui::glmVec2(ImGui::GetWindowPos()) + glm::vec2(75, 75));
				drawList->AddRectFilled(ImGui::GetWindowPos(), size, ImGui::ColorConvertFloat4ToU32(backgroundColor));

				ImGui::SetCursorPos(ImGui::imVec2(ImGui::glmVec2(ImGui::GetCursorPos()) + glm::vec2(75 / 2 - 25)));
				ImGui::Image(ImTextureID(this->textureId), ImVec2(50, 50));
				ImGui::SetCursorPos(ImGui::imVec2(ImGui::glmVec2(ImGui::GetCursorPos()) - glm::vec2(0, 10)));
				ImGui::Text(this->name.c_str());

			}
			ImGui::EndChild();


			if (ImGui::IsItemClicked()) {
				if (filesystem::is_directory(filesystem::path{ directory })) {
					Editor::Gui::FileExplorer::currentDirectory = directory;
					Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
				}
			}
			currentPos.x += iconSize + spacing;
			if (currentPos.x + iconSize > ImGui::GetWindowPos().x + ImGui::GetWindowWidth()) {
				currentPos.x = ImGui::GetCursorScreenPos().x;
				currentPos.y += iconSize + spacing;
			}
		}

	}
}