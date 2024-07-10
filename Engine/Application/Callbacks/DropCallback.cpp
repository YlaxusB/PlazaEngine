#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Core/ModelLoader/ModelLoader.h"
#include "Engine/Application/Application.h"
#include "Editor/ModelImporter/ModelImporter.h"
#include "Editor/Gui/FileExplorer/FileExplorer.h"
#include "Editor/GUI/guiMain.h"
#include "Engine/Core/AssetsManager/Importer/AssetsImporter.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"
#include "Editor/GUI/AssetsImporterWindow/AssetsImporterWindow.h"
using Plaza::Editor::Gui;
void Plaza::ApplicationClass::Callbacks::dropCallback(GLFWwindow* window, int count, const char** paths) {
	for (unsigned int i = 0; i < count; i++) {

		vector<string> supportedFormats = ModelLoader::supportedFormats;
		string fileExtension = filesystem::path{ paths[i] }.extension().string();
		bool foundMatch = std::any_of(supportedFormats.begin(), supportedFormats.end(),
			[&fileExtension](const std::string& str) {
				return str == fileExtension;
			});
		//if (foundMatch) {
		std::string fileName = filesystem::path{ paths[i] }.stem().string();

		std::string path = paths[i];
		if (fileExtension == Standards::metadataExtName)
			path = Metadata::DeSerializeMetadata(paths[i]).mPath.string();

		if (AssetsLoader::mSupportedLoadFormats.find(fileExtension) != AssetsLoader::mSupportedLoadFormats.end())
			AssetsLoader::LoadAsset(AssetsManager::GetAsset(std::filesystem::path{ path }));
		else
			Application->mEditor->mGui.mAssetsImporter.OpenAssetsImporter(path, "");
		//AssetsImporter::ImportAsset(path);

	//ModelLoader::LoadModelToGame(paths[i], fileName);
	//Editor::ModelImporter::ImportModel(Gui::FileExplorer::currentDirectory, fileName, fileExtension, paths[i]);
		//}
	}
}