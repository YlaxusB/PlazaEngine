#pragma once
#include "Engine/Core/PreCompiledHeaders.h"
#include "Engine/Components/Rendering/Material.h"
#include "Editor/GUI/Utils/Utils.h"
#include "Engine/Application/Serializer/FileSerializer/FileSerializer.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
namespace Plaza::Editor {
	static class MaterialFileInspector {
	public:
		static Material* material;
		static File* lastFile;
		MaterialFileInspector(File* file) {
			if (!material || file != lastFile) {
				material = MaterialFileSerializer::DeSerialize(file->directory);
				if (material->uuid && Application->activeScene->materials.find(material->uuid) != Application->activeScene->materials.end())
					material = Application->activeScene->materials.at(material->uuid).get();
			}
			ImGui::Text(file->directory.c_str());

			ImGui::ColorPicker4("Diffuse", &material->diffuse.rgba.x);
			ImGui::ColorPicker4("Specular", &material->specular.rgba.x);
			if (ImGui::Button("Difusse Texture")) {
				material->diffuse.path = FileDialog::OpenFileDialog(".jpeg");
				material->diffuse.rgba = glm::vec4(INFINITY);
				material->diffuse.id = ModelLoader::TextureFromFile(material->diffuse.path);
			}
			if (ImGui::Button("Normal Texture")) {
				material->normal.path = FileDialog::OpenFileDialog(".jpeg");
				material->normal.rgba = glm::vec4(INFINITY);
				material->normal.id = ModelLoader::TextureFromFile(material->normal.path);
			}
			if (ImGui::Button("Metalic Texture")) {
				material->metalness.path = FileDialog::OpenFileDialog(".jpeg");
				material->metalness.rgba = glm::vec4(INFINITY);
				material->metalness.id = ModelLoader::TextureFromFile(material->metalness.path);
			}
			if (ImGui::Button("Roughness Texture")) {
				material->roughness.path = FileDialog::OpenFileDialog(".jpeg");
				material->roughness.rgba = glm::vec4(INFINITY);
				material->roughness.id = ModelLoader::TextureFromFile(material->roughness.path);
			}
			if (ImGui::Button("Height Texture")) {
				material->height.path = FileDialog::OpenFileDialog(".jpeg");
				material->height.rgba = glm::vec4(INFINITY);
				material->height.id = ModelLoader::TextureFromFile(material->height.path);
			}

			//if (Application->activeScene->materials.find(material->uuid) != Application->activeScene->materials.end()) {
			//	Application->activeScene->materials.at(material->uuid) = std::make_shared<Material>(*new Material(*material));
			//}
			if (ImGui::Button("Apply")) {
				MaterialFileSerializer::Serialize(file->directory, material);
			}

			lastFile = file;
		}
	};
}

inline Material* Plaza::Editor::MaterialFileInspector::material = nullptr;
inline File* Plaza::Editor::MaterialFileInspector::lastFile = nullptr;