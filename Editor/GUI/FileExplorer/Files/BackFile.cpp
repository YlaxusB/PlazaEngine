#pragma once
#include "BackFile.h"
#include "Editor/GUI/FileExplorer/FileExplorer.h"

namespace Plaza {
	namespace Editor {
		void BackFile::DoubleClick() {
			Editor::Gui::FileExplorer::currentDirectory = filesystem::path{ Gui::FileExplorer::currentDirectory }.parent_path().string();
			Gui::FileExplorer::UpdateContent(Gui::FileExplorer::currentDirectory);
		}

		void BackFile::Delete() {

		}

		void BackFile::Rename(std::string oldPath, std::string newPath) {

		}

		void BackFile::Move(std::string oldPath, std::string newPath) {

		}

		void BackFile::Copy() {

		}

		void BackFile::Paste() {

		}

		void BackFile::Popup() {

		}
	}
}