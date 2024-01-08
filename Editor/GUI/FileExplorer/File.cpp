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
				// Draw background
				ImDrawList* drawList = ImGui::GetWindowDrawList();
				ImVec2 size = ImGui::imVec2(ImGui::glmVec2(ImGui::GetWindowPos()) + glm::vec2(75, 75));
				drawList->AddRectFilled(ImGui::GetWindowPos(), size, ImGui::ColorConvertFloat4ToU32(backgroundColor));

				// Image
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
					/* TODO:UNCOMMENT THIS */
					//if (ImGui::InputTextEx("##FileNameInput", "File Name", buf, 512, ImVec2(50, 20), ImGuiInputTextFlags_EnterReturnsTrue, nullptr, nullptr, startSelected)) {
					//	changingName = "";
					//	//Utils::Filesystem::ChangeFileName(Editor::Gui::FileExplorer::currentDirectory + "\\" + name, Editor::Gui::FileExplorer::currentDirectory + "\\" + buf);
					//}

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
				else {
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
					ImGui::TextWrapped(this->name.c_str());
					ImGui::SameLine();
					ImGui::Text("...");
					ImGui::PopStyleVar();
					ImGui::PopStyleVar();
				}

			}
			Popup::FileExplorerFilePopup::Update(this);
			ImGui::EndChild();

			// Open, or add the file to the selected files map when user clicked on a file
			if (ImGui::IsItemClicked()) {
				// Clicked on a folder and is not holding ctrl
				if (ImGui::IsMouseDoubleClicked(0)) {
					// Handle double click on folders
					if (filesystem::is_directory(filesystem::path{ directory }) && glfwGetKey(Application->Window->glfwWindow, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS) {
						Editor::Gui::FileExplorer::currentDirectory = directory;
						Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
						Editor::selectedFiles.clear();
					} // Handle double click on .cs files
					else if (filesystem::path{ directory }.extension() == ".cs") {
						/* Get Devenv path */
						std::string getDevenvCommand = (Application->enginePath + "/vendor/vsWhere/vswhere -latest -requires Microsoft.Component.MSBuild -find Common7/IDE/devenv.exe");
						// Open a pipe to capture the command output
						FILE* pipe = _popen(getDevenvCommand.c_str(), "r");
						if (!pipe) {
							std::cerr << "Error: Unable to execute the command." << std::endl;
						}
						char buffer[1024];
						std::string devenvPath = "";

						// Read the command output character by character
						int c;
						while ((c = fgetc(pipe)) != EOF) {
							// Filter out newline characters
							if (c != '\n' && c != '\r') {
								devenvPath += static_cast<char>(c);
							}
						}
						// Close the pipe
						_pclose(pipe);

						/* "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe" /command "File.OpenFile" "Speed Runers.sln" scriptchola.cs */
						std::string projectPath = std::filesystem::path{ Application->activeProject->directory + "/" + Application->activeProject->name }.replace_extension(".sln").string();
						std::string scriptPath = this->directory;
						std::string openCsFileCommand = "\"\"" + devenvPath + "\" \"" + projectPath + "\" \"" + scriptPath + "\"\"";
						for (size_t i = 0; i < openCsFileCommand.length(); ++i) {
							if (openCsFileCommand[i] == '\\') {
								openCsFileCommand[i] = '/';
							}
						}
						std::cout << "OpenCsFileCommand: " << openCsFileCommand.c_str() << "\n";
						//system(openCsFileCommand.c_str());

						FILE* pipe2 = _popen(openCsFileCommand.c_str(), "r");
						if (!pipe2) {
							std::cerr << "Error: Unable to execute the command." << std::endl;
						}
					}
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