#include "Engine/Core/PreCompiledHeaders.h"
#include "FileExplorerFilePopup.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Engine/Core/Script.h"

#include "Editor/ScriptManager/ScriptManager.h"
#include "Editor/GUI/FileExplorer/File.h"
#include "FileExplorerPopup.h"
namespace Plaza::Editor {
	void Popup::FileExplorerFilePopup::UpdateContent() {
		if (ImGui::MenuItem("Delete"))
		{
			for (const auto& [key, value] : Editor::selectedFiles) {
				Utils::Filesystem::DeleteFileF(value->directory);
			}
		}
	}
	void Popup::FileExplorerFilePopup::Update() {
		if (ImGui::BeginPopupContextWindow("FileExplorerFilePopup"))
		{
			UpdateContent();
			//FileExplorerPopup::UpdateContent();
			ImGui::EndPopup();
		}
	}
}