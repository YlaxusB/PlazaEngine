#pragma once
#include "Engine/Components/Core/CoreComponents.h"
#include "Editor/GUI/FileExplorer/File.h"
#include "Engine/Core/Scripting/FieldManager.h"
namespace Plaza {
	namespace Editor
	{
		extern Entity* selectedGameObject;
		extern std::map<std::string, File*> selectedFiles;
		static std::map<uint64_t, std::map<std::string, std::map<std::string, Field*>>> lastSavedScriptsFields;
	}
}