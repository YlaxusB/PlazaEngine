#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/Material.h"
#include "Editor/GUI/Utils/Utils.h"
#include "Engine/Application/Serializer/FileSerializer/FileSerializer.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
#include "Engine/Core/Renderer/Renderer.h"
namespace Plaza::Editor {
	static class MaterialFileInspector {
	public:
		static Material* material;
		static File* lastFile;

		Texture* LoadTextureButton() {
			Asset* asset = AssetsManager::GetAssetOrImport(FileDialog::OpenFileDialog(".jpeg"));
			if (asset)
				return Application->mRenderer->LoadTexture(asset->mPath.string(), asset->mAssetUuid);
			else
				return AssetsManager::mTextures.find(1)->second;
		}

		MaterialFileInspector(File* file) {
			if (!material || file != lastFile) {
				if (AssetsManager::GetAsset(file->directory))
				{
					auto materialIt = Application->activeScene->materials.find(AssetsManager::GetAsset(file->directory)->mAssetUuid);
					if (materialIt != Application->activeScene->materials.end())
						material = materialIt->second.get();
				}
			}

			if (!material)
			{
				ImGui::Text("Material not found");
				return;
			}

			ImGui::Text(file->directory.c_str());

			ImGui::ColorPicker4("Diffuse", &material->diffuse->rgba.x);
//			ImGui::ColorPicker4("Specular", &material->specular->rgba.x);
			if (ImGui::Button("Difusse Texture")) {
				material->diffuse = LoadTextureButton();
				//    material->diffuse.path = FileDialog::OpenFileDialog(".jpeg");
				//    material->diffuse.rgba = glm::vec4(INFINITY);
				//    material->diffuse.Load() = ModelLoader::TextureFromFile(material->diffuse.path);
			}
			if (ImGui::Button("Normal Texture")) {
				material->normal = LoadTextureButton();
			}
			if (ImGui::Button("Metalic Texture")) {
				material->metalness = LoadTextureButton();
			}
			if (ImGui::Button("Roughness Texture")) {
				material->roughness = LoadTextureButton();
			}
			if (ImGui::Button("Height Texture")) {
				material->height = LoadTextureButton();
			}

			ImGui::DragFloat("Roughness: ", &material->roughnessFloat, 0.0f, 1.0f);
			ImGui::DragFloat("Metalness: ", &material->metalnessFloat, 0.0f, 1.0f);
			ImGui::DragFloat("Diffuse Intensity: ", &material->intensity, 0.0f, 100.0f);

			ImGui::InputFloat("Flip X", &material->flip.x);
			ImGui::InputFloat("Flip Y", &material->flip.y);

			//if (Application->activeScene->materials.find(material->uuid) != Application->activeScene->materials.end()) {
			//	Application->activeScene->materials.at(material->uuid) = std::make_shared<Material>(*new Material(*material));
			//}
			if (ImGui::Button("Apply")) {
				material->name = std::filesystem::path{ file->directory }.stem().string();
				AssetsSerializer::SerializeMaterial(material, file->directory);
				//MaterialFileSerializer::Serialize(file->directory, material);
			}

			lastFile = file;
		}
	};
}

inline Material* Plaza::Editor::MaterialFileInspector::material = nullptr;
inline File* Plaza::Editor::MaterialFileInspector::lastFile = nullptr;