#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/Material.h"
#include "Editor/GUI/Utils/Utils.h"
#include "Engine/Core/Renderer/Renderer.h"
#include "Engine/Application/FileDialog/FileDialog.h"
#include "Editor/GUI/FileExplorer/File.h"
#include "Editor/GUI/Utils/DataVisualizer.h"

namespace Plaza::Editor {
	static class MaterialFileInspector {
	public:
		static Material* material;
		static File* lastFile;

		Texture* LoadTextureButton(std::string outDirectory = "") {
			Asset* asset = AssetsManager::GetAssetOrImport(FileDialog::OpenFileDialog(".jpeg"), {}, outDirectory);

			if (asset)
				return Application::Get()->mRenderer->LoadTexture(asset->mAssetPath.string(), asset->mAssetUuid);
			else
				return AssetsManager::mTextures.find(1)->second;
		}

		MaterialFileInspector(File* file) {
			if (!material || file != lastFile) {
				if (AssetsManager::GetAsset(file->directory))
				{
					auto materialIt = AssetsManager::mMaterials.find(AssetsManager::GetAsset(file->directory)->mAssetUuid);
					if (materialIt != AssetsManager::mMaterials.end())
						material = materialIt->second.get();
				}
			}

			if (!material)
			{
				ImGui::Text("Material not found");
				return;
			}

			ImGui::Text(file->directory.c_str());

			ImGui::BeginTable("MaterialFileInspectorTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
			if (Utils::AddTableTexture("Diffuse: ", material->diffuse, ImGuiSliderFlags_None))
				material->diffuse = LoadTextureButton(std::filesystem::path{ material->mAssetPath }.parent_path().string());
			if (Utils::AddTableTexture("Normal: ", material->normal, ImGuiSliderFlags_None))
				material->normal = LoadTextureButton(std::filesystem::path{ material->mAssetPath }.parent_path().string());
			if (Utils::AddTableTexture("Roughness: ", material->roughness, ImGuiSliderFlags_None))
				material->roughness = LoadTextureButton(std::filesystem::path{ material->mAssetPath }.parent_path().string());
			if (Utils::AddTableTexture("Metalness: ", material->metalness, ImGuiSliderFlags_None))
				material->metalness = LoadTextureButton(std::filesystem::path{ material->mAssetPath }.parent_path().string());
			if (Utils::AddTableTexture("Height: ", material->height, ImGuiSliderFlags_None))
				material->height = LoadTextureButton(std::filesystem::path{ material->mAssetPath }.parent_path().string());
			ImGui::EndTable();

			ImGui::InputFloat("Flip X", &material->flip.x);
			ImGui::InputFloat("Flip Y", &material->flip.y);

			if (ImGui::Button("Apply")) {
				material->mAssetName = std::filesystem::path{ file->directory }.stem().string();
				AssetsSerializer::SerializeMaterial(material, file->directory, Application::Get()->mSettings.mMaterialSerializationMode);
			}

			lastFile = file;
		}
	};
}

inline Material* Plaza::Editor::MaterialFileInspector::material = nullptr;
inline Editor::File* Plaza::Editor::MaterialFileInspector::lastFile = nullptr;