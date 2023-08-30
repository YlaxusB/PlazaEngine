#pragma once
#include "Engine/Components/Core/CoreComponents.h"
#include "Editor/GUI/FileExplorer/File.h"
namespace Plaza {
	namespace Editor
	{
		extern Entity* selectedGameObject;
		extern std::map<std::string, File*> selectedFiles;
	}
}