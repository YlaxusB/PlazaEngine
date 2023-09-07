#include "Engine/Core/PreCompiledHeaders.h"
#include "FileExplorer.h"
#include "Editor/GUI/Popups/FileExplorerPopup.h"

namespace Plaza {
	namespace Editor {
		std::string Gui::FileExplorer::currentDirectory = "";
		void Gui::FileExplorer::UpdateGui() {
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			Entity* selectedGameObject = Editor::selectedGameObject;

			// Set the window to be the content size + header size
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
			//windowFlags |= ImGuiWindowFlags_NoScrollbar;
			if (ImGui::Begin("File Explorer", &Gui::isFileExplorerOpen, windowFlags)) {
				if (ImGui::IsWindowFocused())
					Application->focusedMenu = "File Explorer";
				if (ImGui::IsWindowHovered())
					Application->hoveredMenu = "File Explorer";
				File::currentPos = ImVec2(-1.0f, 1.0f);

				ImGui::BeginGroup();

				// Create all the icons
				unsigned int index = 0;
				for (File file : files) {
					if (file.name != "")
						file.Update();
					if (index == 0) {
						// Back Button Click
						if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && filesystem::path{currentDirectory}.parent_path().string().starts_with(Application->activeProject->directory)) {
							Editor::Gui::FileExplorer::currentDirectory = filesystem::path{ currentDirectory }.parent_path().string();
							Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
						}
					}
					index++;
				}

				Popup::FileExplorerPopup::Update();
				
				ImGui::EndGroup();
			}

			ImGui::End();
			ImGui::PopStyleColor();


		}

		std::vector<File> Gui::FileExplorer::files = std::vector<File>();
		void Gui::FileExplorer::UpdateContent(std::string folderPath) {
			files.clear();
			namespace fs = std::filesystem;

			// Back Button
			std::string currentDirectory = Gui::FileExplorer::currentDirectory;
			const std::string& currentDirectoryPath = filesystem::path{ currentDirectory }.string();
			File backFile = File();
			backFile.directory = currentDirectory + "\\asd.back";
			backFile.extension = ".back";
			backFile.name = ".back";
			backFile.textureId = Icon::textures.at("").id;
			files.push_back(backFile);

			for (const auto& entry : fs::directory_iterator(folderPath)) {
				std::string filename = entry.path().filename().string();
				std::string extension = entry.path().extension().string();
				if (entry.path().stem().string() == filename)
					extension = filename;
				// Check if its a folder
				if (fs::is_directory(entry.path())) {
					File newFile = File();
					newFile.directory = entry.path().string();
					newFile.name = filename;
					newFile.extension = extension;
					newFile.textureId = Icon::textures.at("").id;
					files.push_back(newFile);
				}
				else if (entry.path().extension() != ".dll") {
					File newFile = File();
					newFile.directory = entry.path().string();
					newFile.name = filename;
					newFile.extension = extension;
					const auto& it = Icon::textures.find(extension);
					if (it != Icon::textures.end()) {
						newFile.textureId = it->second.id;
					}
					else {
						newFile.textureId = Icon::textures.at(".notFound").id;
					}
					files.push_back(newFile);
				}
			}
		}
	}
}