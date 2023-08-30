#include "Editor.h"
#include "Engine/Core/PreCompiledHeaders.h"
#include "Editor/GUI/FileExplorer/File.h"
//Entity* Plaza::Editor::selectedGameObject = nullptr;
namespace Plaza {
    namespace Editor {
        Entity* selectedGameObject = nullptr;
        std::map<std::string, File*> selectedFiles = std::map<std::string, File*>();
    }
}