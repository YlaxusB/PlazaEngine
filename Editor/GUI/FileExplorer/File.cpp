#include "Engine/Core/PreCompiledHeaders.h"
#include "File.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Editor/GUI/Popups/FileExplorerFilePopup.h"
unsigned int startSelected = 0;
namespace Plaza {
	namespace Editor {
		std::map<std::string, IconTexture> Icon::textures = std::map<std::string, IconTexture>();

		float Editor::File::iconSize = 75.0f;
		float Editor::File::spacing = 5.0f;
		ImVec2 Editor::File::currentPos = ImVec2(-1.0f, 1.0f);
		bool Editor::File::firstFocus = false;
		std::string Editor::File::changingName = "";

		void Editor::File::Update() {

			if (currentPos.x == -1.0f) {
				currentPos = ImGui::GetCursorScreenPos();
			}
			ImGui::SetCursorScreenPos(currentPos);

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoFocusOnAppearing;
			windowFlags |= ImGuiWindowFlags_NoScrollbar;

			if (ImGui::BeginChild(ImGui::GetID(this->name.c_str()), ImVec2(75, 75), false, windowFlags)) {
				ImVec4 backgroundColor = editorStyle.hierarchyBackgroundColor;

				if (ImGui::IsWindowHovered() || Editor::selectedFiles.contains(directory)) {
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
				if (changingName == name) {
					if (firstFocus) {
						ImGui::SetKeyboardFocusHere();
						startSelected = std::filesystem::path{ name }.stem().string().length();
					}

					char buf[512];
					strcpy_s(buf, name.c_str());
					if (ImGui::InputTextEx("##FileNameInput", "File Name", buf, 512, ImVec2(50, 20), ImGuiInputTextFlags_EnterReturnsTrue, nullptr, nullptr, startSelected)) {
						changingName = "";
						//Utils::Filesystem::ChangeFileName(Editor::Gui::FileExplorer::currentDirectory + "\\" + name, Editor::Gui::FileExplorer::currentDirectory + "\\" + buf);
					}

					if (!ImGui::IsItemActive() && !firstFocus) {
						changingName = "";
						Utils::Filesystem::ChangeFileName(Editor::Gui::FileExplorer::currentDirectory + "\\" + name, Editor::Gui::FileExplorer::currentDirectory + "\\" + buf);
						name = buf;
						Gui::FileExplorer::UpdateContent(Editor::Gui::FileExplorer::currentDirectory);
					}

					if (firstFocus) {
						firstFocus = false;
					}
				}
				else
					ImGui::Text(this->name.c_str());

			}
			Popup::FileExplorerFilePopup::Update();
			ImGui::EndChild();

			// Open, or add the file to the selected files map when user clicked on a file
			if (ImGui::IsItemClicked()) {
				// Clicked on a folder and is not holding ctrl
				if (filesystem::is_directory(filesystem::path{ directory }) && glfwGetKey(Application->Window->glfwWindow, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS) {
					Editor::Gui::FileExplorer::currentDirectory = directory;
					Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
					Editor::selectedFiles.clear();
				}
				else {
					// Clicked on something and is pressing control
					if (glfwGetKey(Application->Window->glfwWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
						// Check if this file is already in the selected files, if yes, then remove it from the map
						if (Editor::selectedFiles.contains(this->name)) {
							Editor::selectedFiles.erase(this->name);
						}
						else {
							// Its not on the selected files map, so just add it
							Editor::selectedFiles.emplace(directory, new File(*this));
						}
					}
					else { // Clicked without control
						Editor::selectedFiles.clear();
						Editor::selectedFiles.emplace(directory, new File(*this));
					}
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