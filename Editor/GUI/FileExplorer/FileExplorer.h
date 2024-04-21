#pragma once
#include "Editor/GUI/guiMain.h"
#include "Editor/GUI/FileExplorer/File.h"
namespace Plaza {
	namespace Editor {
		class Gui::FileExplorer {
		public:
			static inline bool breakFilesLoop = false;
			static std::vector<std::unique_ptr<File>> files;
			static std::string currentDirectory;
			static void Init();
			static void UpdateGui();
			static void DrawFile(File* file);
			/// <summary>
			/// Read the project directory to get the present files
			/// </summary>
			static void UpdateContent(std::string folderPath);
		};
	}
}