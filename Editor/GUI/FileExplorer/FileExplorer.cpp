#include "Engine/Core/PreCompiledHeaders.h"
#include "FileExplorer.h"


namespace Engine {
	namespace Editor {
		std::string Gui::FileExplorer::directory = "";
		void Gui::FileExplorer::UpdateGui() {
			ApplicationSizes& appSizes = *Application->appSizes;
			ApplicationSizes& lastAppSizes = *Application->lastAppSizes;
			GameObject* selectedGameObject = Editor::selectedGameObject;

			// Set the window to be the content size + header size
			ImGuiWindowFlags  sceneWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove;
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

			ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
			//windowFlags |= ImGuiWindowFlags_NoScrollbar;

			if (ImGui::Begin("File Explorer", &Gui::isFileExplorerOpen, windowFlags)) {
				if (ImGui::IsWindowFocused())
					Application->focusedMenu = "File Explorer";
				for (File file : files) {
					Icon::Update(file.directory, file.extension, file.name);
				}
			}

			ImGui::End();
			ImGui::PopStyleColor();
		}
		std::vector<File> Gui::FileExplorer::files = std::vector<File>();
		void Gui::FileExplorer::UpdateContent(std::string folderPath) {
			files.clear();
			directory = Application->activeProject->directory;

			namespace fs = std::filesystem;
			for (const auto& entry : fs::directory_iterator(folderPath)) {
				std::string filename = entry.path().filename().string();
				std::string extension = entry.path().extension().string();
				std::cout << "File: " << filename << "  Extension: " << extension << std::endl;
				// Check if its a folder
				if (fs::is_directory(entry)) {
					File newFile = File();
					newFile.directory = entry.path().string();
					newFile.name = filename;
					files.push_back(newFile);
				}
				else {
					File newFile = File();
					newFile.directory = entry.path().string();
					newFile.name = filename;
					newFile.extension = extension;
					files.push_back(newFile);
				}
			}
		}
	}
}