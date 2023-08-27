#include "Engine/Core/PreCompiledHeaders.h"
#include "FileExplorerPopup.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Engine/Core/Script.h"
namespace Plaza::Editor {
	void Popup::FileExplorerPopup::Update() {
		if (ImGui::BeginPopupContextWindow("FileExplorerPopup"))
		{
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("Folder"))
				{
					Utils::Filesystem::CreateFolder(Gui::FileExplorer::currentDirectory + "\\Unnamed");
				}

				if (ImGui::BeginMenu("Script"))
				{
					if (ImGui::MenuItem("C++ Script"))
					{
						Utils::Filesystem::CreateNewFile(Gui::FileExplorer::currentDirectory + "\\Unnamed.cpp");
						Application->activeProject->scripts.push_back(Script(Gui::FileExplorer::currentDirectory + "\\Unnamed.cpp", "Unnamed.cpp"));
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}