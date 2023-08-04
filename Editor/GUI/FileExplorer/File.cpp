#include "Engine/Core/PreCompiledHeaders.h"
#include "File.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
namespace Engine {
	namespace Editor {
		std::vector<IconTexture> Icon::textures = std::vector<IconTexture>();
		float Icon::iconSize = 75.0f;
		float Icon::spacing = 5.0f;
		ImVec2 Icon::currentPos = ImVec2(-1.0f, 1.0f);

		void Editor::Icon::Update(std::string directory, std::string extension, std::string fileName, File file) {
			
			if (currentPos.x == -1.0f) {
				currentPos = ImGui::GetCursorScreenPos();
			}
			ImGui::SetCursorScreenPos(currentPos);

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoFocusOnAppearing;
			windowFlags |= ImGuiWindowFlags_NoScrollbar;
			
			if (ImGui::BeginChild(ImGui::GetID(fileName.c_str()), ImVec2(75, 75), false, windowFlags)) {
				ImVec4 backgroundColor = editorStyle.hierarchyBackgroundColor;

				if (ImGui::IsWindowHovered()) {
					backgroundColor = editorStyle.treeNodeHoverBackgroundColor;
				}

				if (ImGui::BeginDragDropSource()) {
					File* filePtr = new File(file);
					ImGui::Text(filePtr->name.c_str());
					ImGui::Text(filePtr->directory.c_str());
					ImGui::SetDragDropPayload(Gui::scenePayloadName.c_str(), &filePtr, sizeof(Editor::File&));
					ImGui::EndDragDropSource();
				}
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 size = ImGui::imVec2(ImGui::glmVec2(ImGui::GetWindowPos()) + glm::vec2(75, 75));
				drawList->AddRectFilled(ImGui::GetWindowPos(), size, ImGui::ColorConvertFloat4ToU32(backgroundColor));

				ImGui::SetCursorPos(ImGui::imVec2(ImGui::glmVec2(ImGui::GetCursorPos()) + glm::vec2(75 / 2 - 25)));
				if (filesystem::is_directory(fileName)) {
					ImGui::Image(ImTextureID(textures.front().id), ImVec2(50, 50));
				}
				else {
					bool found = false;
					for (int i = 0; i < textures.size(); i++) {
						if (textures[i].extension == filesystem::path{ fileName }.extension()) {
							found = true;
							ImGui::Image(ImTextureID(textures[i].id), ImVec2(50, 50));
						}
					}
					if (!found)
						ImGui::Image(ImTextureID(textures[1].id), ImVec2(50, 50));
				}
				ImGui::SetCursorPos(ImGui::imVec2(ImGui::glmVec2(ImGui::GetCursorPos()) - glm::vec2(0, 10)));
				ImGui::Text(fileName.c_str());

			}
			ImGui::EndChild();


			if (ImGui::IsItemClicked()) {
				if (filesystem::is_directory(filesystem::path{directory})) {
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