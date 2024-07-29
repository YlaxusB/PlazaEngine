#include "Engine/Core/PreCompiledHeaders.h"
#include "CallbacksHeader.h"
#include "Engine/Application/Application.h"
#include "Editor/Gui/FileExplorer/FileExplorer.h"
#include "Editor/GUI/guiMain.h"
#include "Engine/Core/AssetsManager/Importer/AssetsImporter.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"
#include "Editor/GUI/AssetsImporterWindow/AssetsImporterWindow.h"
using Plaza::Editor::Gui;
void Plaza::Callbacks::dropCallback(GLFWwindow* window, int count, const char** paths) {
	for (unsigned int i = 0; i < count; i++) {

		string fileExtension = filesystem::path{ paths[i] }.extension().string();
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