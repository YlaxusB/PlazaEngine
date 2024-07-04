#pragma once
#include "DragDrop.h"
#include "Editor/GUI/FileExplorer/File.h"
#include "Engine/Core/AssetsManager/AssetsManager.h"
#include "Engine/Core/AssetsManager/Loader/AssetsLoader.h"

namespace Plaza {
	namespace Editor {
		class DropFileScene {
		public:
			static void Drop(File* file) {
				if (file->extension == Standards::modelExtName) {
					AssetsLoader::LoadAsset(AssetsManager::GetAsset(file->directory));
				}
			}
		};
	}
}