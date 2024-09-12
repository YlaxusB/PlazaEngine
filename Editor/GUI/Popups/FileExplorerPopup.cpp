#include "Engine/Core/PreCompiledHeaders.h"
#include "FileExplorerPopup.h"
#include "Editor/GUI/Utils/Filesystem.h"
#include "Engine/Core/Script.h"

#include "Editor/ScriptManager/ScriptManager.h"
#include "Editor/GUI/FileExplorer/File.h"
#include "Engine/Application/Serializer/FileSerializer/FileSerializer.h"
namespace Plaza::Editor {
	void Popup::FileExplorerPopup::UpdateContent() {
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Folder"))
			{
				Editor::File::changingName = Utils::Filesystem::CreateFolder(Gui::FileExplorer::currentDirectory + "\\Unnamed");
				Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
				Editor::File::changingName = std::filesystem::path{ Editor::File::changingName }.filename().string();
				//Editor::File::changingName = "Unnamed";
				Editor::File::firstFocus = true;
			}

			if (ImGui::BeginMenu("Script"))
			{
				if (ImGui::MenuItem("C# Script"))
				{
					Editor::File::changingName = Utils::Filesystem::CreateNewFile(Gui::FileExplorer::currentDirectory + "\\Unnamed.cs");
					Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
					Editor::File::changingName = std::filesystem::path{ Editor::File::changingName }.filename().string();
					Editor::File::firstFocus = true;
					//Application::Get()->activeProject->scripts.push_back(Script(Gui::FileExplorer::currentDirectory + "\\Unnamed.cs", "Unnamed.cs"));
					ScriptManager::NewCsScript(Gui::FileExplorer::currentDirectory + "\\" + Editor::File::changingName);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Rendering"))
			{
				if (ImGui::MenuItem("Material")) {
					Editor::File::changingName = Utils::Filesystem::CreateNewFile(Gui::FileExplorer::currentDirectory + "\\Unnamed" + Standards::materialExtName);
					Editor::File::changingName = std::filesystem::path{ Editor::File::changingName }.filename().string();
					Editor::File::firstFocus = true;
					Material* material = new Material();
					material->mAssetUuid = Plaza::UUID::NewUUID();
					//AssetsManager::mMaterials.emplace(material.uuid, std::make_shared<Material>(material));
					AssetsManager::AddMaterial(material);
					std::string path = Gui::FileExplorer::currentDirectory + "\\" + Editor::File::changingName;
					AssetsManager::NewAsset(material->mAssetUuid, AssetType::MATERIAL, path);
					AssetsSerializer::SerializeMaterial(material, path);
					//MaterialFileSerializer::Serialize(path, material);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
	}
	void Popup::FileExplorerPopup::Update() {
		if (ImGui::BeginPopupContextWindow("FileExplorerPopup"))
		{
			UpdateContent();
			ImGui::EndPopup();
		}
	}
}