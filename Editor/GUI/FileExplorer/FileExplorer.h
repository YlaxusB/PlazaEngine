#pragma once
#include "Editor/GUI/guiMain.h"
#include "Editor/GUI/FileExplorer/File.h"
namespace Engine {
	namespace Editor {
		class Gui::FileExplorer {
		public:
			static std::vector<File> files;
			static std::string directory;
			static void Init();
			static void UpdateGui();
			/// <summary>
			/// Read the project directory to get the present files
			/// </summary>
			static void UpdateContent(std::string folderPath);
		};
	}
}