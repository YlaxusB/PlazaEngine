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
		MaterialFileInspector(File* file) {
			if (!material || file != lastFile) {
				material = Application->activeScene->materials.at(AssetsManager::GetAsset(file->directory)->mAssetUuid).get();//MaterialFileSerializer::DeSerialize(file->directory);
				//if (material->uuid && Application->activeScene->materials.find(material->uuid) != Application->activeScene->materials.end())
				//	material = Application->activeScene->materials.at(material->uuid).get();
			}
			ImGui::Text(file->directory.c_str());

			ImGui::ColorPicker4("Diffuse", &material->diffuse->rgba.x);
			ImGui::ColorPicker4("Specular", &material->specular->rgba.x);
			if (ImGui::Button("Difusse Texture")) {
				Asset* asset = AssetsManager::GetAssetOrImport(FileDialog::OpenFileDialog(".jpeg"));
				material->diffuse = Application->mRenderer->LoadTexture(asset->mPath.string(), asset->mAssetUuid);
				//    material->diffuse.path = FileDialog::OpenFileDialog(".jpeg");
				//    material->diffuse.rgba = glm::vec4(INFINITY);
				//    material->diffuse.Load() = ModelLoader::TextureFromFile(material->diffuse.path);
			}
			if (ImGui::Button("Normal Texture")) {
				Asset* asset = AssetsManager::GetAssetOrImport(FileDialog::OpenFileDialog(".jpeg"));
				material->normal = Application->mRenderer->LoadTexture(asset->mPath.string(), asset->mAssetUuid);
			}
			if (ImGui::Button("Metalic Texture")) {
				Asset* asset = AssetsManager::GetAssetOrImport(FileDialog::OpenFileDialog(".jpeg"));
				material->metalness = Application->mRenderer->LoadTexture(asset->mPath.string(), asset->mAssetUuid);
			}
			if (ImGui::Button("Roughness Texture")) {
				Asset* asset = AssetsManager::GetAssetOrImport(FileDialog::OpenFileDialog(".jpeg"));
				material->roughness = Application->mRenderer->LoadTexture(asset->mPath.string(), asset->mAssetUuid);
			}
			if (ImGui::Button("Height Texture")) {
				Asset* asset = AssetsManager::GetAssetOrImport(FileDialog::OpenFileDialog(".jpeg"));
				material->height = Application->mRenderer->LoadTexture(asset->mPath.string(), asset->mAssetUuid);
			}

			ImGui::DragFloat("Roughness: ", &material->roughnessFloat, 0.0f, 1.0f);
			ImGui::DragFloat("Metalness: ", &material->metalnessFloat, 0.0f, 1.0f);
			ImGui::DragFloat("Diffuse Intensity: ", &material->intensity, 0.0f, 100.0f);

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