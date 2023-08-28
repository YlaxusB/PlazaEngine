#include "Engine/Core/PreCompiledHeaders.h"
#include "FileExplorerPopup.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Engine/Core/Script.h"

#include "Editor/ScriptManager/ScriptManager.h"
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
					if (ImGui::MenuItem("C# Script"))
					{
						Utils::Filesystem::CreateNewFile(Gui::FileExplorer::currentDirectory + "\\Unnamed.cs");
						//Application->activeProject->scripts.push_back(Script(Gui::FileExplorer::currentDirectory + "\\Unnamed.cs", "Unnamed.cs"));
						ScriptManager::NewCsScript(Gui::FileExplorer::currentDirectory + "\\Unnamed.cs");
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}
			ImGui::EndPopup();
		}
	}
}